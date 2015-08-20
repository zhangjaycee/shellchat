/**************************************************************
*******                                                 *******
***                                                         ***
**   Filename: chat_server.c          Time: 15-07-19-16:14   **
**   Email:    zhjcyx@gmail.com       By: Jaycee             **
***                                                         ***
*******                                                 *******
**************************************************************/

#define MAXLINE 25
#define SA struct sockaddr
#define SERV_PORT 9877
#define LISTENQ 5

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<termios.h>
#include<sys/ioctl.h>
#include<signal.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include"jc_wr.h"
#include"jc_err.h"


int term_cols=0;

void str_send(int sockfd)
{
	char sendline[MAXLINE];
	char key;
	char * flag="-> ";
	char * esc="exit\n";
	for(;;){
		fputs(flag,stdout);
		if(fgets(sendline,MAXLINE,stdin)!=NULL){
			if(strcmp(esc,sendline)==0){
				break;
			}
			writen(sockfd,sendline,MAXLINE);
			//printf("[server]\tsend ok.\n");
		}else{
			break;
		}
	}
}
void str_recv(int sockfd)
{
	int space_num,i;
	char recvline[MAXLINE];
	char * flag=" <-[client]\n-> ";

	for(;;){
		memset(recvline,'\0',sizeof(recvline));
		if(read(sockfd,recvline,MAXLINE)==0){
			//err_quit("[server]\tterminated prematurely");
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
		//printf("[%d chars received]",strlen(recvline));
		fputs(recvline,stdout);
		fputs(flag,stdout);
		fflush(stdout);
	}
}

static void sig_winch()
{
	struct winsize size;
			
	if(ioctl(STDIN_FILENO,TIOCGWINSZ,(char *)&size)<0){
		err_sys("[server]TIOCGWINSZ error");
	}
	term_cols=size.ws_col;
}

int main()//server
{
	/****获取终端窗口改变信号*****/
	if(isatty(STDIN_FILENO)==0){
		exit(1);
	}
	if(signal(SIGWINCH,sig_winch)==SIG_ERR){
		err_sys("signal error");
	}
		sig_winch();
	/*****************************/
    pid_t childpid;
    int listenfd,connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr,servaddr;
    listenfd=socket(AF_INET,SOCK_STREAM,0);//AF_INET:ipv4  SOCK_STREAM:字节流
    bzero(&servaddr,sizeof(servaddr));//将servaddr置零
    servaddr.sin_family = AF_INET;
    //INADDR_ANY本机地址0.0.0.0,可以代表任意所有网卡的地址
    //htonl函数把本机序转为网络序,用于long型；htons函数功能类似，用于short型
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(SERV_PORT);
    bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
    listen(listenfd,LISTENQ);
    for(;;){
        clilen = sizeof(cliaddr);
        connfd=accept(listenfd,(SA *)&cliaddr,&clilen);
        if((childpid=fork())==0){//child
            close(listenfd);
            str_send(connfd);
			exit(0);
      //  }else if((childpid=fork())==0){
        }else{
			str_recv(connfd);
			exit(0);
		}
        close(connfd);
    }
}
