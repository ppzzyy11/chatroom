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




int main(int argc, char* argv[])
{
    struct event_base* base;
    struct evconnlistener* listener;
    struct event* signal_event;

    struct sockaddr_in addr;


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
	addr.sin_port = htons(8976);

    //struct bufferevent* ;
    HERE


	event_base_free(base);

	printf("done\n");
	return 0;
}
