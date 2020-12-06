/* TCPServer.cpp - main */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "conio.h"

#define	WSVERS	MAKEWORD(2, 0)

#pragma comment(lib,"ws2_32.lib")  //使用winsock 2.2 library
/*------------------------------------------------------------------------
 * main - Iterative TCP server for TIME service
 *------------------------------------------------------------------------
 */
 
void num_to_string(int a,int b,int c,int d,char *ss)
{
	char aa[10],bb[10],cc[10],dd[10];
	itoa(a,aa,10);
	itoa(b,bb,10);
	itoa(c,cc,10);
	itoa(d,dd,10);
	char point[2]=".";
	strcpy(ss,aa);
	strcat(ss,point);
	strcat(ss,bb);
	strcat(ss,point);
	strcat(ss,cc);
	strcat(ss,point);
	strcat(ss,dd);
}
int main(int argc, char *argv[]) 
/* argc: 命令行参数个数， 例如：C:\> TCPServer 8080 
                     argc=2 argv[0]="TCPServer",argv[1]="8080" */
{
	struct	sockaddr_in fsin;	    /* the from address of a client	  */
	SOCKET	msock, ssock;		    /* master & slave sockets	      */
	WSADATA wsadata; 
	char	*service = "50500";
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
    int	    alen;			        /* from-address length		        */
	char	*pts;			        /* pointer to time string	        */
	time_t	now;			        /* current time			            */
    char buffer[1000];// 缓冲区 
    char temp[1000],temp1[1000]; 
	if(WSAStartup(WSVERS, &wsadata)==SOCKET_ERROR)// 加载winsock library。WSVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本
	{
		printf("服务器启动失败！\n");
	}
	else
	{
		printf("服务器已启动！\n");
	}						
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														// 返回：要监听套接字的描述符或INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// 从&sin开始的长度为sizeof(sin)的内存清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// 监听所有(接口的)IP地址。
	sin.sin_port = htons((u_short)atoi(service));		// 监听的端口号。atoi--把ascii转化为int，htons--主机序到网络序(host to network，s-short 16位)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // 绑定监听的IP地址和端口号

	listen(msock, 5);                                   // 建立长度为5的连接请求队列，并把到来的连接请求加入队列等待处理。

    while(1){ 		                                   // 检测是否有按键,如果没有则进入循环体执行
       alen = sizeof(struct sockaddr);                         // 取到地址结构的长度
	   ssock = accept(msock, (struct sockaddr *)&fsin, &alen); // 如果在连接请求队列中有连接请求，则接受连接请求并建立连接，返回该连接的套接字，否则，本语句被阻塞直到队列非空。fsin包含客户端IP地址和端口号
	    
       memset(buffer,0,sizeof(buffer));//初始化 
	   recv(ssock,buffer,1000,0);//接收客户端数据
	   if((buffer[0]=='E'||buffer[0]=='e')&&(buffer[1]='S'||buffer[1]=='s')&&(buffer[2]=='C'||buffer[2]=='c')) 
	   //输入esc退出 
	   {
	   	  printf("服务器已关闭\n");
	   	  break;
	   } 
	   (void) time(&now);                                      // 取得系统时间
       pts = ctime(&now);                                      // 把时间转换为字符串
       printf("收到消息：");
       printf("\n");
	   printf("内容：%s",buffer);
       printf("时间：%s",pts);
       send(ssock, buffer, strlen(buffer), 0);
       send(ssock, pts, strlen(pts), 0);
	   printf("客户端IP地址：%d.%d.%d.%d\n",fsin.sin_addr.S_un.S_un_b.s_b1,fsin.sin_addr.S_un.S_un_b.s_b2,fsin.sin_addr.S_un.S_un_b.s_b3,fsin.sin_addr.S_un.S_un_b.s_b4);
	   printf("客户端端口号：%d\n",fsin.sin_port);
	   num_to_string(fsin.sin_addr.S_un.S_un_b.s_b1,fsin.sin_addr.S_un.S_un_b.s_b2,fsin.sin_addr.S_un.S_un_b.s_b3,fsin.sin_addr.S_un.S_un_b.s_b4,temp);
	   send(ssock,temp,strlen(temp),0); 
	   itoa(fsin.sin_port,temp1,10);
	   send(ssock,temp1,strlen(temp1),0);
//	   int cc = send(ssock, pts, strlen(pts), 0);              // 第二个参数指向发送缓冲区，第三个参数为要发送的字节数，第四个参数一般置0，返回值：>=0 实际发送的字节数，0 对方正常关闭，SOCKET_ERROR 出错。
//       printf("%s", pts);                                      // 显示发送的字符串
       (void) closesocket(ssock);                              // 关闭连接套接字
     }
    (void) closesocket(msock);                                 // 关闭监听套接字
     WSACleanup();                                             // 卸载winsock library
     printf("按回车键继续...");
 	 getchar();										// 等待任意按键
	 getchar();


}

