/*
 * client.cpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
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


using std::string;
using std::vector;
const unsigned int MSG_MAX_LEN=1200;
static void clnt_write_cb(struct bufferevent *, void*);
static void clnt_read_cb(struct bufferevent*, void*);
static void clnt_error_cb(struct bufferevent*, short, void*);
static void signal_cb(evutil_socket_t, short, void*);
static void* write_thread(void* para);

int main(int argc,char* argv[])
{
    if(argc<3)
    {
        fprintf(stderr,"Usage:%s <ip> <port>.\n",argv[0]);
        return -1;
    }

    struct event_base* base;
    struct bufferevent* bev;
    struct event* signal_event;
    struct sockaddr_in addr;


    //event_base
    base=event_base_new();
    if(!base)
    {
        fprintf(stderr,"Could not initialize libevent!\n");
        return 1;
    }else
    {
        fprintf(stdout,"Initialize libevent.\n");
    }

    //addr
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(argv[1]);
    addr.sin_port=htons(atoi(argv[2]));

    //bufferevent
    bev=bufferevent_socket_new(base,-1,BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        fprintf(stderr,"Could not initialize bufferevent!\n");
        return 1;
    }else
    {
        fprintf(stdout,"Initialize bufferevent!\n");
    }
    //set callback of bufferevent
	bufferevent_setcb(bev, clnt_read_cb, clnt_write_cb, clnt_error_cb, NULL);
    //set option of bufferevent
	bufferevent_enable(bev, EV_WRITE|EV_READ);

    //connect
    if(-1==bufferevent_socket_connect(bev,(struct sockaddr*)&addr,sizeof(addr)))
    {
        fprintf(stderr,"Could not connect.\n");
        return 1;
    }else//call clnt_error_cb with BEV_EVENT_CONNECTED
    {
        fprintf(stdout,"Could not connect.\n");
    }

    signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}else
    {
        fprintf(stdout,"Create a signal_event.\n");
    }

    pthread_t p;
    if(0!=pthread_create(&p,NULL,write_thread,(void*)bev))
    {
        fprintf(stderr,"Could not create a pthread.\n");
    }else
    {

        event_base_dispatch(base);
    }

    pthread_cancel(p);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;




    return 0;
}

static void clnt_write_cb(struct bufferevent *bev, void* para)
{
    //fprintf(stdout,"write cb is called.\n");
}

static void clnt_read_cb(struct bufferevent *bev, void* para)
{

    struct evbuffer *input = bufferevent_get_input(bev);
    char msg[MSG_MAX_LEN];
    memset(msg,0,sizeof(msg));
    int len=0;


    //recv the TCP stream, send it to the converter, get the return commands including (1. client's bev; 2. string to send.)
    while(len=evbuffer_remove(input,msg,sizeof(msg)-1))
    {
        fprintf(stdout,"%s\n",msg);
        memset(msg,0,sizeof(msg));
    }
    fflush(stdout);

}

static void clnt_error_cb(struct bufferevent *bev, short event, void* para)
{
    if(event&BEV_EVENT_CONNECTED)
    {
        fprintf(stderr,"Connect to server.\n");
    }else
    {
        fprintf(stderr,"Error happens.\n");
    }
}

static void signal_cb(evutil_socket_t fd, short event, void* para)
{
    EXIT=1;
	struct event_base *base = (struct event_base*)para;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}
static void* write_thread(void* para)
{
    fprintf(stdout,"write thread begin.\n");

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);//can be terminate by call pthread_cancel(th) from main

    struct bufferevent* bev=(struct bufferevent*) para;
    struct evbuffer* output=bufferevent_get_output(bev);

    string str;
    while(1)
    {
        //complete you business logics here.
        //
        str="";
        fprintf(stdout,"User interface.\nInput:");
        std::cin>>str;
        sleep(1);
        std::cout<<"Input "<<str<<"\n";
        if(0!=evbuffer_add(output,str.c_str(),str.size()))
        {
            ;
        }else
        {
            ;
        }
    }
    fprintf(stdout,"write thread end.\n");
}
