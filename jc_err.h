/**********************************
** filename: jc_err.h
** time: 2015 02 03   byJaycee
** Email: zhjcyx@gmail.com
**********************************/
#define MAXLINE0 4096
#include<stdio.h>
#include<stdarg.h>//可变参数的头文件
#include<string.h>
#include<errno.h>

/*doit好像就是do it的意思。。。*/
static void err_doit(int errnoflag, int error, const char *fmt, va_list ptr)
{
	char buf[MAXLINE0];
	vsnprintf(buf,MAXLINE-1,fmt,ptr);//variable string print format(max n)
	if(errnoflag){
		snprintf(buf+strlen(buf),MAXLINE-strlen(buf)-1,":%s",strerror(error));
	}
	strcat(buf,"\n");
	fflush(stdout);
	fputs(buf,stderr);
	fflush(NULL);
}
/*ret是return的意思*/
void err_ret(const char *fmt,...)
{	
	va_list ptr;//va_list 实际是char * 类型
	va_start(ptr,fmt);//第二个参数是"..."前面的最后一个参数
	err_doit(1,errno,fmt,ptr);
	va_end(ptr);
}
void err_sys(const char *fmt,...)
{	
	va_list ptr;//va_list 实际是char * 类型
	va_start(ptr,fmt);//第二个参数是"..."前面的最后一个参数
	err_doit(1,errno,fmt,ptr);
	va_end(ptr);
	exit(1);
}
static void err_quit(const char * fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    err_doit(0,0,fmt,ap);
    va_end(ap);
    exit(1);
}
