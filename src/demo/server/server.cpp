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

#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

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


	listener = evconnlistener_new_bind(base, accept_cb, (void *)base,
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

	printf("done\n");
	return 0;
}


static void
accept_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (struct event_base*)user_data;
	struct bufferevent *bev;

    //build a new bufferevent for the new client.
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, clnt_read_cb, clnt_write_cb, clnt_error_cb, NULL);
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
        fprintf(stdout,"%s\n",msg);
    }
    fflush(stdout);

    //send string to the client, dev
    char* data="ACK\n";
    bufferevent_write(bev,data,strlen(data)+1);

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
