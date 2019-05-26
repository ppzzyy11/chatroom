/*
 * client.cpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 *
 *
 * WishList:
 *
 * - passwd input echo "*"
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

#include "datastructure.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <event2/bufferevent_ssl.h>

#define CA_CERT_FILE "CA.crt"
#define CLIENT_CERT_FILE "client.crt"
#define CLIENT_KEY_FILE "client.key"

using std::string;
using std::vector;
const unsigned int MSG_MAX_LEN=1200;
static void clnt_write_cb(struct bufferevent *, void*);
static void clnt_read_cb(struct bufferevent*, void*);
static void clnt_error_cb(struct bufferevent*, short, void*);
static void signal_cb(evutil_socket_t, short, void*);
static void* write_thread(void* para);
string AppMessage(unsigned char oper, vector<string> strs);

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

    SSL_CTX* ctx=NULL;
    SSL* ssl=NULL;

    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();


    //SSL version2, v3
    ctx=SSL_CTX_new(SSLv23_method());
    if(ctx==NULL)
    {
        fprintf(stderr,"SSL_CTX_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //verify server's certificate.
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);


    //load CA.crt
    fprintf(stdout,"SSL_CTX_load_verify_locations starts!\n");
    if(!SSL_CTX_load_verify_locations(ctx,CA_CERT_FILE,NULL))
    {
        fprintf(stderr,"SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    //load client's certificate.>>>>
    fprintf(stdout,"SSL_CTX_use_certificate_file starts!\n");
    if(SSL_CTX_use_certificate_file(ctx, CLIENT_CERT_FILE, SSL_FILETYPE_PEM)<=0)
    {
        fprintf(stderr,"SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }


    //load client's private key, if the server want to verify the client.
    fprintf(stdout,"SSL_CTX_use_PrivateKey_file starts!\n");
    if(SSL_CTX_use_PrivateKey_file(ctx,CLIENT_KEY_FILE,SSL_FILETYPE_PEM)<=0)
    {
        fprintf(stderr,"SSL_CTX_load_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }


    //If the private key right?
    fprintf(stdout,"SSL_CTX_check_private_key starts!!\n");
    if(!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr,"SSL_CTX_check_private_key error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

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

    fprintf(stdout,"Initialize addr!\n");
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(argv[1]);
    addr.sin_port=htons(atoi(argv[2]));

    ssl=SSL_new(ctx);
    fprintf(stdout,"Initialize openssl socket!\n");
    bev = bufferevent_openssl_socket_new(base, -1, ssl,
            BUFFEREVENT_SSL_CONNECTING,
            BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        fprintf(stderr,"Could not initialize bufferevent!\n");
        return 1;
    }else
    {
        fprintf(stdout,"Initialize bufferevent!\n");
    }
    //set callback of bufferevent
	bufferevent_setcb(bev, clnt_read_cb, clnt_write_cb, clnt_error_cb, base);
    //set option of bufferevent
	bufferevent_enable(bev, EV_WRITE|EV_READ);

    //connect
    if(0!=bufferevent_socket_connect(bev,(struct sockaddr*)&addr,sizeof(addr)))
    {
        fprintf(stderr,"Could not connect.\n");
        return 1;
    }else//call clnt_error_cb with BEV_EVENT_CONNECTED
    {
        fprintf(stdout,"connect.\n");
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

    SSL_free(ssl);
    SSL_CTX_free(ctx);

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
    }else if((event&BEV_EVENT_ERROR)||(event&BEV_EVENT_READING)||(event&BEV_EVENT_WRITING))
    {
        fprintf(stderr,"Error happens.\n");

        struct event_base *base = (struct event_base*)para;
        struct timeval delay = { 0, 0 };


        event_base_loopexit(base, &delay);
    }
}

static void signal_cb(evutil_socket_t fd, short event, void* para)
{
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

    string cmds="\
Command to choose:\n\
1. Register \n\
2. Login(Set Account)\n\
3. Quit\n\
4. GetRooms\n\
5. BuildRoom\n\
6. CloseRoom\n\
7. ChangeRoomAdmin\n\
8. JoinRoom\n\
9. QuitRoom\n\
10. SendMessageInRoom\n\
11. GetUsersInRoom\n\
12. PrivateMessage\n\
13. KickUser\n\
//14. GetInRoom(Set Room Id locally)\n\
//15. SetUserFroPrivateMessage(Set Locally)\n\
16. GetRoom\n\
Input number to choose:\n\
";
    int choice;
    unsigned char oper;
    Account account="";
    Passwd passwd="";
    Nickname nickname="";
    vector<string> strs;
    string command;
    string tmp;
    using std::cout;
    using std::cin;
    while(1)
    {
        //complete you business logics here.
        choice=0;
        oper=0;
        strs.clear();
        command="";
        std::cout<<cmds;
        std::cin>>choice;
        std::cout<<"Please input...\n";
        if(account==""&&!(choice==1||choice==2))
        {
            cout<<"Login first!\n";
        }else
        {
            switch (choice)
            {
                case 1:
                    oper=1;
                    cout<<"account:";
                    cin>>account;
                    cout<<"passwd:";
                    cin>>passwd;
                    cout<<"nickname:";
                    cin>>nickname;
                    strs.push_back(account);
                    strs.push_back(passwd);
                    strs.push_back(nickname);
                    nickname=passwd=account="";
                    break;


                case 2:
                    oper=2;
                    cout<<"account:";
                    cin>>account;
                    cout<<"passwd:";
                    cin>>passwd;
                    strs.push_back(account);
                    strs.push_back(passwd);
                    break;

                case 3:

                    break;

                case 4:
                    oper=4;
                    strs.push_back(account);
                    break;

                case 5:
                    oper=5;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Room Name:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Room Description:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Room Passwd:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 6:
                    oper=6;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 7:
                    oper=7;
                    strs.push_back(account);


                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"New admin's account:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 8:
                    oper=8;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Room Passwd:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 9:
                    oper=9;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 10:
                    oper=10;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Message:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 11:
                    oper=11;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    break;

                case 12:
                    oper=12;
                    strs.push_back(account);

                    cout<<"Receiver account:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Message:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                case 13:
                    oper=13;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);

                    cout<<"Kciked's account:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                //case 14:

                    //break;

                //case 15:

                    //break;

                case 16:

                    oper=16;
                    strs.push_back(account);

                    cout<<"Room Id:";
                    cin>>tmp;
                    strs.push_back(tmp);
                    break;

                default:
                    cout<<"wrong number!\n";
                    break;
            }
            if(strs.size()!=0)
            {
                command=AppMessage(oper,strs);
                evbuffer_add(output,command.c_str(),command.size());
            }
        }
        sleep(1);
    }
    fprintf(stdout,"write thread end.\n");
}

string AppMessage(unsigned char oper, vector<string> strs)
{
    string rtn="";

    rtn.push_back(oper);

    for(auto str:strs)
    {
        rtn=rtn+str+'\n';
    }

    int len=rtn.size();
    char len0=len%256;
    char len1=(len/256)%256;
    rtn=len0+rtn;
    rtn=len1+rtn;

    return rtn;

}
