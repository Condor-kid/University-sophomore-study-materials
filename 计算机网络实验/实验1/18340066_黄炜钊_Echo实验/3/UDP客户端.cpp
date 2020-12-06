/* UDPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>

#define	BUFLEN		2000                  // 缓冲区大小
#define WSVERS		MAKEWORD(2, 2)        // 指明版本2.2 
#pragma comment(lib,"ws2_32.lib")         // 加载winsock 2.2 Llibrary

int main(int argc, char *argv[])
{
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in toAddr;	        /* an Internet endpoint address	*/
	char	buf[BUFLEN+1];   		    /* buffer for one line of text	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/
	int	cc;			                    /* recv character count		    */
	char	*pts;			            /* pointer to time string	    */
	time_t	now;			            /* current time			        */

    char buffer1[BUFLEN+1],buffer2[BUFLEN+1],buffer3[BUFLEN+1];
    int mysize=sizeof(toAddr);
	WSADATA wsadata;
    WSAStartup(WSVERS, &wsadata);       /* 启动某版本Socket的DLL        */	

    sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP);

	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons((u_short)atoi(service));    //atoi：把ascii转化为int. htons：主机序(host)转化为网络序(network), s--short
	toAddr.sin_addr.s_addr = inet_addr(host);           //如果host为域名，需要先用函数gethostbyname把域名转化为IP地址


//	memset(buf,'e',BUFLEN);  //fill 1000 bytes with 'e'
//	buf[BUFLEN] = '\0';
    (void) time(&now);                                      // 取得系统时间
    pts = ctime(&now);                                      // 把时间转换为字符串
    //memcpy(buf,pts,strlen(pts));
    printf("时间：");
    printf("%s",pts);
    printf("输入消息：");
	scanf("%s",buffer1); 
	sendto(sock,buffer1,BUFLEN,0,(SOCKADDR *)&toAddr, sizeof(toAddr));
    sendto(sock,pts, BUFLEN, 0,(SOCKADDR *)&toAddr, sizeof(toAddr));
    printf("收到的消息：");
    printf("%s",buffer1);
    printf("\n"); 
    recvfrom(sock,buffer2,BUFLEN,0,(SOCKADDR *)&toAddr, &mysize);
    recvfrom(sock,buffer3,BUFLEN,0,(SOCKADDR *)&toAddr, &mysize);
    printf("客户端地址：");
    printf("%s",buffer2);
    printf("\n");
    printf("客户端端口：");
    printf("%s",buffer3);
    printf("\n");
   // if (cc == SOCKET_ERROR){
   //     printf("发送失败，错误号：%d\n", WSAGetLastError());
   //  }
   //else{
   //		printf("发送成功!\r\n");
   //}

	closesocket(sock);
	WSACleanup();       	          /* 卸载某版本的DLL */  

	printf("按任意键退出...");
	getchar();
	getchar(); 

}
