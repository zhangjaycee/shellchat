/**************************************************************
*******                                                 *******
***                                                         ***
**   Filename: chat_client.c          Time: 15-08-19-16:14   **
**   Email:    zhjcyx@gmail.com       By: Jaycee             **
***                                                         ***
*******                                                 *******
**************************************************************/

#define MAXLINE 50 
#define SA struct sockaddr
#define SERV_PORT 9877

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<signal.h>
#include<termios.h>
#include<sys/ioctl.h>
#include"jc_wr.h"
#include"jc_err.h"
#include<time.h>

int term_cols=0;
/******打开聊天记录文件*******/
FILE * logfp;
time_t t=0;


void write_log(char * name,char * buffer)
{
        time_t t_new;
        char timebuf[50];
        char flag_log[20];
        memset(flag_log,'\0',sizeof(flag_log));
        time(&t_new);
        if(t_new-t>30){
                strftime(timebuf,sizeof(timebuf),"[%F %T]",localtime(&t_new));
                fputs(timebuf,logfp);
                fprintf(logfp,"\n");
                time(&t);//更新t
        }
        fprintf(logfp,"    (%s)-> ",name);
        fputs(buffer,logfp);
        fflush(logfp);
        fprintf(logfp,"\n");
}

void str_send(FILE *fp,int sockfd)
{
        char sendline[MAXLINE];
        fflush(stdin);
        char * flag="-> ";
        for(;;){
                fputs(flag,stdout);
                if(fgets(sendline,MAXLINE,stdin)!=NULL){
                        writen(sockfd,sendline,MAXLINE);
                        //remove \n
                        sendline[strlen(sendline)-1]='\0';
                        write_log("I",sendline);
                }else{
                        break;
                }
        }
}
void str_recv(FILE *fp,int sockfd)
{
        int space_num,i;
        char * flag=" <-[server]\n-> ";
        char recvline[MAXLINE];

        for(;;){
                if(read(sockfd,recvline,MAXLINE)==0){
                   // err_quit("[client]\tserver terminated prematurely");
                        exit(0);
                }
                
                space_num=term_cols-strlen(recvline)-strlen(flag)-15;
                if(space_num<0){
                        space_num=0;
                }
                printf("\r");
                for(i=0;i<space_num;i++){
                        printf(" ");
                }
                recvline[strlen(recvline)-1]='\0';
                fputs(recvline,fp);
                fputs(flag,fp); 
                fflush(fp);
                //write chat log
                write_log("server",recvline);
        }
}

static void sig_winch()
{
        struct winsize size;
        
        if(ioctl(STDIN_FILENO,TIOCGWINSZ,(char *)&size)<0){
                err_sys("[client]TIOCGWINSZ error");
        }
        //printf("[client]\tSIGWINCH received\n");
        //printf("[client]\t%drows,%dcols\n",size.ws_row,size.ws_col);
        
        term_cols=size.ws_col;
}

int main(int  argc, char **argv )
{
        logfp=fopen("chatlog_client","a+");
        char ip_addr;
        /****获取终端窗口改变信号*****/
        if(isatty(STDIN_FILENO)==0){
                exit(1);
        }
        if(signal(SIGWINCH,sig_winch)==SIG_ERR){
                err_sys("signal error");
        }
        sig_winch();
        /*****************************/
        int connfd;
        pid_t childpid;
        struct sockaddr_in servaddr;
        if(argc!=2){
                printf("usage:tcpcli <IPaddress>\ndefault:127.0.0.1 start\n");
        }
        connfd = socket(AF_INET,SOCK_STREAM,0);
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        if(argc!=2){
                inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
        }else{
                inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
        }
        fprintf(logfp,"\n\n############New Dialogue Start!#############\n");
        fflush(logfp);
        for(;;){
                int conn_st=connect(connfd,(SA *)&servaddr,sizeof(servaddr));
                if(conn_st==-1){
                         printf("connect error\n");
                         close(connfd);
                         exit(0);
                 }
                if((childpid=fork())==0){//child
                        str_send(stdin,connfd);
                        exit(0);
                }else{//parent
                        str_recv(stdout,connfd);
                        exit(0);
                }
                close(connfd);
                fprintf(logfp,"\n#############Dialogue Finished!#############\n\n");
        }
}


