/*================================================================================

    #Author:       puzeyu
    #Mail:         puzeyu@bupt.edu.cn
    #FileType:     C source code
    #Enviroment:   Linux & Ubuntu
    #Tool:         Vim & GCC
    #Date:         1
    #Descprition:

================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

pthread_mutex_t mutex;
char quit=0;

void Read(int rd);
void Write(int wr);

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

int main(int argc, char* argv[])
{

    if(argc<3)
    {
        fprintf(stderr,"Usage: %s <ip> <port>\n",argv[0]);
        return 1;
    }

    int sock=socket(AF_INET,SOCK_STREAM,0);

    pthread_mutex_init(&mutex,NULL);

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(argv[1]);
    addr.sin_port=htons(atoi(argv[2]));

    if(0>connect(sock,(struct sockaddr *)&addr,sizeof(addr)))
    {
        fprintf(stderr,"connect() error!\n");
        return 1;
    }else
    {
        fprintf(stdout,"connect().\n");
    }


    pthread_t p_rd,p_wr;

    pthread_create(&p_wr,0,Write,sock);
    pthread_create(&p_rd,0,Read,sock);

    pthread_join(p_rd,NULL);
    pthread_join(p_wr,NULL);

    close(sock);



    return 0;
}


void Read(int rd)
{
    char txt[999];
    bzero(txt,sizeof(txt));

    while(quit==0)
    {
        recv(rd,txt,sizeof(txt),0);
        {
            pthread_mutex_lock(&mutex);
            fprintf(stdout,"\n");
            fprintf(stdout,txt);
            pthread_mutex_unlock(&mutex);
        }
        bzero(txt,sizeof(txt));
    }

    {
        pthread_mutex_lock(&mutex);
        fprintf(stdout,"Close read stream\n");
        pthread_mutex_unlock(&mutex);
    }


    return 0;
}


void Write(int wr)
{

    char txt[999];

    {
        pthread_mutex_lock(&mutex);
        fprintf(stdout,"Input Message to send(EXIT! to exit):");
        pthread_mutex_unlock(&mutex);
    }

    while(1)
    {

        bzero(txt,sizeof(txt));
        char ch;
        int i=0;
        while(i<sizeof(txt)-1&&(ch=getchar())!='\n')
        {
            txt[i]=ch;
            i++;
        }

        if(strcmp(txt,"EXIT!")==0)
        {
            bzero(txt,sizeof(txt));
            txt[0]=-1;
            send(wr,txt,strlen(txt),0);
            break;
        }
        send(wr,txt,strlen(txt),0);
    }


    {
        pthread_mutex_lock(&mutex);
        fprintf(stdout,"Close write stream\n");
        pthread_mutex_unlock(&mutex);
    }
}

