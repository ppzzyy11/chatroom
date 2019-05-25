/*
 * server.c
 * Copyright (C) 2019-05-06 puzeyu <puzeyu@bupt.edu.cn>
 * Distributed under terms of the MIT license.
 * Description:
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

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <event2/bufferevent_ssl.h>

#define CA_CERT_FILE "CA.crt"
#define SERVER_CERT_FILE "server.crt"
#define SERVER_KEY_FILE "server.key"

const size_t MSG_MAX_LEN= 1025;



static void accept_cb(struct evconnlistener*,evutil_socket_t,struct sockaddr *,int socklen,void*);

static void clnt_write_cb(struct bufferevent *, void*);
static void clnt_read_cb(struct bufferevent*, void*);
static void clnt_error_cb(struct bufferevent*, short, void*);

static void signal_cb(evutil_socket_t, short, void*);

int main(int argc, char* argv[])
{
    if(argc<2)
    {
        fprintf(stderr,"Usage: %s <port>\n",argv[0]);
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



	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}else
    {
        fprintf(stdout, "Initilialize libevent.\n");
    }

	memset(&addr,0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port = htons(atoi(argv[1]));
    fflush(stdout);

    ctx=SSL_CTX_new(SSLv23_method());

    if(ctx==NULL)
    {
        fprintf(stderr,"SSL_CTX_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //don't verify client's certificate.
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);


    //load CA.crt
    fprintf(stdout,"SSL_CTX_load_verify_locations starts!\n");
    if(!SSL_CTX_load_verify_locations(ctx,CA_CERT_FILE,NULL))
    {
        fprintf(stderr,"SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //load server.crt
    fprintf(stdout,"SSL_CTX_use_certificate_file starts!\n");
    if(!SSL_CTX_use_certificate_file(ctx,SERVER_CERT_FILE,SSL_FILETYPE_PEM))
    {
        fprintf(stderr,"SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }


    //load private key
    fprintf(stdout,"SSL_CTX_use_PrivateKey_file starts!\n");
    if(SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY_FILE, SSL_FILETYPE_PEM)<=0)
    {
        fprintf(stderr,"SSL_CTX_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //verify the private key
    if(!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr,"SSL_CTX_check_private_key error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }


    listener = evconnlistener_new_bind(base, accept_cb, (void *)ctx,
            LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&addr,
	    sizeof(addr));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}else
    {
        fprintf(stdout, "Create an evconnlistener.\n");
    }

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}else
    {
        fprintf(stdout,"Create a signal_event.\n");
    }

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

    SSL_CTX_free(ctx);

	printf("done\n");
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

	bufferevent_setcb(bev, clnt_read_cb, clnt_write_cb, clnt_error_cb, bev);
	bufferevent_enable(bev, EV_WRITE|EV_READ);


    fprintf(stdout,"new connection from client %d.\n",fd);
}

static void clnt_write_cb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
	}
}
static void clnt_read_cb(struct bufferevent* bev, void *user_data)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    char msg[MSG_MAX_LEN];
    memset(msg,0,sizeof(msg));
    int len=0;


    //recv the TCP stream, send it to the converter, get the return commands including (1. client's bev; 2. string to send.)
    fprintf(stdout,"Received:");
    while(len=evbuffer_remove(input,msg,sizeof(msg)-1))
    {
        fprintf(stdout,"%s",msg);
        memset(msg,0,sizeof(msg));
    }
    fflush(stdout);


}

static void clnt_error_cb(struct bufferevent* bev,short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
        //send a message to the application layer to remove the client's information.
        //get a vector of messages(send STRING to WHOM)
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
    }else if(events & BEV_EVENT_CONNECTED){
		printf("Connection finished.\n",events);
        return ;
    }
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);

}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (struct event_base*)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}
