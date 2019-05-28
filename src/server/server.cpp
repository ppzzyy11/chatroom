/*
 * server.c
 * Copyright (C) 2019-05-06 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 *
 * Description:
 *      Implement a server using libevent that accept TCP connections. The server should write/read/close any TCP connection.
 *
 *    To implement:
 *      listener_connect_event
 *          accept_callback
 *          error_callback
 *
 *      for every connection, build a socket_event for each(close on free)
 *          read_callback
 *          write_callback
 *          error_callback
 *
 *          done some
 *
 *      remove information of a client when the client quit:
 *          in the error_callback
 *
 *      send a message to the application layer and receive commands (send STRING to WHOM)
 *
 *    How:
 *
 *    To optimize:
 *
 */

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include <vector>
#include <cstring>
#include <cstdlib>

#include "converter.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <event2/bufferevent_ssl.h>

#define CA_CERT_FILE "CA.crt"
#define SERVER_CERT_FILE "server.crt"
#define SERVER_KEY_FILE "server.key"

const size_t MSG_MAX_LEN= 1025;

Converter con("data.sav");


static void accept_cb(struct evconnlistener*,evutil_socket_t,struct sockaddr *,int socklen,void*);

static void clnt_write_cb(struct bufferevent *, void*);
static void clnt_read_cb(struct bufferevent*, void*);
static void clnt_error_cb(struct bufferevent*, short, void*);

static void signal_cb(evutil_socket_t, short, void*);

int main(int argc, char* argv[])
{
    if(argc<2)
    {
        Log("Usage:"+string(argv[0])+" <port>\n",2);
        return -1;
    }

    struct event_base* base;
    struct evconnlistener* listener;
    struct event* signal_event;

    struct sockaddr_in addr;

    SSL_CTX* ctx;
    SSL* ssl;

    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
        Log("Could not initialize libevent!\n",2);
		return 1;
	}else
    {
        Log("Initilialize libevent.\n",1);
    }

	memset(&addr,0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port = htons(atoi(argv[1]));

    ctx=SSL_CTX_new(SSLv23_method());

    if(ctx==NULL)
    {
        Log("SSL_CTX_new error!\n",2);
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //don't verify client's certificate.
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);


    //load CA.crt
    Log("SSL_CTX_load_verify_locations starts!\n",1);
    if(!SSL_CTX_load_verify_locations(ctx,CA_CERT_FILE,NULL))
    {
        Log("SSL_CTX_load_verify_locations error!\n",2);
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //load server.crt

    Log("SSL_CTX_use_certificate_file starts!\n",1);
    if(!SSL_CTX_use_certificate_file(ctx,SERVER_CERT_FILE,SSL_FILETYPE_PEM))
    {
        Log("SSL_CTX_use_certificate_file error!\n",2);
        ERR_print_errors_fp(stderr);
        return -1;
    }


    //load private key
    Log("SSL_CTX_use_PrivateKey_file starts!\n",1);
    if(SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY_FILE, SSL_FILETYPE_PEM)<=0)
    {
        Log("SSL_CTX_use_PrivateKey_file error!\n",2);
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //verify the private key
    if(!SSL_CTX_check_private_key(ctx))
    {
        Log("SSL_CTX_check_private_key error!\n",2);
        ERR_print_errors_fp(stderr);
        return -1;
    }



	listener = evconnlistener_new_bind(base, accept_cb, (void *)ctx,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&addr,
	    sizeof(addr));

	if (!listener) {
		Log("Could not create a listener!\n",2);
		return 1;
	}else
    {
        Log( "Create an evconnlistener.\n",1);
    }

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		Log("Could not create/add a signal event!\n",2);
		return 1;
	}else
    {
        Log("Create a signal_event.\n",1);
    }

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);
    SSL_CTX_free(ctx);
    Log("Server quit.\n",1);

	return 0;
}


static void
accept_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base;
	struct bufferevent *bev;

    SSL_CTX *server_ctx;
    SSL *client_ctx;

    server_ctx=(SSL_CTX*) user_data;
    client_ctx=SSL_new(server_ctx);

    base=evconnlistener_get_base(listener);

    //build a new ssl bufferevent
    bev = bufferevent_openssl_socket_new(base,fd,client_ctx,BUFFEREVENT_SSL_ACCEPTING,BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		Log( "Error constructing bufferevent!",2);
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, clnt_read_cb, clnt_write_cb, clnt_error_cb, bev);
	bufferevent_enable(bev, EV_WRITE|EV_READ);
    con.ConAccept((void*)bev);

    Log("new connection from client "+std::to_string(fd)+".",1);
}

static void clnt_write_cb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
        ;
	}
}
static void clnt_read_cb(struct bufferevent* bev, void *user_data)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    char msg[MSG_MAX_LEN];
    memset(msg,0,sizeof(msg));
    int len=0;


    //recv the TCP stream, send it to the converter, get the return commands including (1. client's bev; 2. string to send.)
    std::vector<char> chs;
    while(len=evbuffer_remove(input,msg,sizeof(msg)-1))
    {
        for(int i=0;i<len;i++)
        {
            chs.push_back(msg[i]);
        }
    }
    std::vector<std::pair<void*,std::string>> rtn=con.ConRead(user_data,chs);

    //send string to the client, dev
    //char* data="ACK\n";
    for(auto par:rtn)
    {
        bufferevent_write((bufferevent*)par.first,par.second.c_str(),strlen(par.second.c_str()));
    }

}

static void clnt_error_cb(struct bufferevent* bev,short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		Log("Connection closed.\n",1);
        //send a message to the application layer to remove the client's information.
        //get a vector of messages(send STRING to WHOM)
	} else if (events & BEV_EVENT_ERROR) {

		Log("Got an error on the connection: "+string(strerror(errno)),2);/*XXX win32*/
    }else if(events & BEV_EVENT_CONNECTED){
		Log("Connection connected..\n",1);
        return ;
    }
	/* None of the other events can happen here, since we haven't enabled*/
	bufferevent_free(bev);
}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (struct event_base*)user_data;
	struct timeval delay = { 2, 0 };

	Log("Caught an interrupt signal; exiting cleanly in two seconds.\n",1);

	event_base_loopexit(base, &delay);
}
