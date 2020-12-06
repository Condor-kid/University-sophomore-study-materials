/* TCPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include<time.h>
#include<process.h>

#define	BUFLEN		2000                  // 缓冲区大小
#define WSVERS		MAKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for TIME service
 *------------------------------------------------------------------------
 */
 SOCKET sock,sockets[50]={NULL};//最多可以容纳50个客户端 
 HANDLE hThread;
 unsigned threadid;

unsigned int __stdcall CHAT(PVOID PM)
{
	char buffer[1000];
	while(1)
	{
		int cc=recv(sock,buffer,1000,0);
		if(cc==SOCKET_ERROR||cc==0)
		{
			printf("无法连接上服务器\n");
			CloseHandle(hThread);
			closesocket(sock);
			break;
		}
		else
		{
			printf("%s\n",buffer);
		}
	}
}
int main(int argc, char *argv[])
{
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	char	buf[BUFLEN+1];   		    /* buffer for one line of text	*/
	//SOCKET	sock;		  	            /* socket descriptor	    	*/
	int	cc;			                    /* recv character count		    */

	//int mysize=0;
	//char buffer1[1000],buffer2[1000],buffer3[1000],buffer4[1000];
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						  //加载winsock library。WSVERS为请求的版本，wsadata返回系统实际支持的最高版本
  
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														  //返回：要监听套接字的描述符或INVALID_SOCKET

    memset(&sin, 0, sizeof(sin));						  // 从&sin开始的长度为sizeof(sin)的内存清0
    sin.sin_family = AF_INET;							  // 因特网地址簇
    sin.sin_addr.s_addr = inet_addr(host);                // 设置服务器IP地址(32位)
    sin.sin_port = htons((u_short)atoi(service));         // 设置服务器端口号  
    int ret=connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // 连接到服务器，第二个参数指向存放服务器地址的结构，第三个参数为该结构的大小，返回值为0时表示无错误发生，返回SOCKET_ERROR表示出错，应用程序可通过WSAGetLastError()获取相应错误代码。
 
	printf("客户端启动！\n");
	hThread=(HANDLE)_beginthreadex(NULL,0,CHAT,NULL,0,&threadid); 
	
	while(1)
	{
		char str[1000];
		gets(str);
		if(!strcmp(str,"exit"))
		{
			break;
		}
		send(sock,str,sizeof(str),0);
		printf("发送成功\n\n"); 
	}
	CloseHandle(hThread);
	closesocket(sock);
	WSACleanup();                     /* 卸载某版本的DLL */
	getchar();
	getchar(); 
	//send(sock,buffer,1000,0);//发送信息
	//if((buffer[0]=='E'||buffer[0]=='e')&&(buffer[1]='S'||buffer[1]=='s')&&(buffer[2]=='C'||buffer[2]=='c')) 
	//{
	//	printf("客户端已关闭\n"); 
	//}
	//else
	//{
	//	recv(sock,buffer1,1000,0);
	//	recv(sock,buffer2,1000,0);
	//	recv(sock,buffer3,1000,0);
	//	recv(sock,buffer4,1000,0);
	//	printf("收到的消息：");
	//	printf("\n"); 
	//	printf("内容：");
	//	printf("%s",buffer1); 
	//	printf("时间："); 
	//	printf("%s",buffer2); 
	//	printf("客户端IP地址：%s",buffer3);
	//	printf("\n");
	//	printf("客户端端口：%s",buffer4);
//	}
//	cc = recv(sock, buf, BUFLEN, 0);                // 第二个参数指向缓冲区，第三个参数为缓冲区大小(字节数)，第四个参数一般设置为0，返回值:(>0)接收到的字节数,(=0)对方已关闭,(<0)连接出错
//  if(cc == SOCKET_ERROR)                          // 出错。其后必须关闭套接字sock
//     printf("Error: %d.\n",GetLastError());
//     else if(cc == 0) {                             // 对方正常关闭
//         printf("Server closed!",buf);  
//    }  else if(cc > 0) {
//         buf[cc] = '\0';	                       // ensure null-termination
//         printf("%s",buf);                         // 显示所接收的字符串
//    }
//    closesocket(sock);                             // 关闭监听套接字
//   WSACleanup();                                  // 卸载winsock library

    printf("按回车键继续...");
	getchar();										// 等待任意按键
	getchar();


}
