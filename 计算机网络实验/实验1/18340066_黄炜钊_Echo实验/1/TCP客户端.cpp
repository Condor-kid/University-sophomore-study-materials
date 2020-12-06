/* TCPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for TIME service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	char	buf[BUFLEN+1];   		    /* buffer for one line of text	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/
	int	cc;			                    /* recv character count		    */

	char buffer[1000];
	int mysize=0;
	printf("����Ҫ���͵���Ϣ��");
	while((buffer[mysize++]=getchar())!='\n')
	{
		continue;
	}
	char buffer1[1000],buffer2[1000],buffer3[1000],buffer4[1000];
	memset(buffer1,0,sizeof(buffer1));
	memset(buffer2,0,sizeof(buffer2));
	memset(buffer3,0,sizeof(buffer3));
	memset(buffer4,0,sizeof(buffer4));
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						  //����winsock library��WSVERSΪ����İ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
  
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //�����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														  //���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

    memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
    sin.sin_family = AF_INET;							  // ��������ַ��
    sin.sin_addr.s_addr = inet_addr(host);                // ���÷�����IP��ַ(32λ)
    sin.sin_port = htons((u_short)atoi(service));         // ���÷������˿ں�  
    int ret=connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // ���ӵ����������ڶ�������ָ���ŷ�������ַ�Ľṹ������������Ϊ�ýṹ�Ĵ�С������ֵΪ0ʱ��ʾ�޴�����������SOCKET_ERROR��ʾ����Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣

    send(sock,buffer,1000,0);//������Ϣ
	if((buffer[0]=='E'||buffer[0]=='e')&&(buffer[1]='S'||buffer[1]=='s')&&(buffer[2]=='C'||buffer[2]=='c')) 
	{
		printf("�ͻ����ѹر�\n"); 
	}
	else
	{
		recv(sock,buffer1,1000,0);
		recv(sock,buffer2,1000,0);
		printf("�յ�����Ϣ��%s",buffer2);
		printf("%s",buffer1); 
	}
//	cc = recv(sock, buf, BUFLEN, 0);                // �ڶ�������ָ�򻺳���������������Ϊ��������С(�ֽ���)�����ĸ�����һ������Ϊ0������ֵ:(>0)���յ����ֽ���,(=0)�Է��ѹر�,(<0)���ӳ���
//  if(cc == SOCKET_ERROR)                          // ����������ر��׽���sock
//     printf("Error: %d.\n",GetLastError());
//     else if(cc == 0) {                             // �Է������ر�
//         printf("Server closed!",buf);  
//    }  else if(cc > 0) {
//         buf[cc] = '\0';	                       // ensure null-termination
//         printf("%s",buf);                         // ��ʾ�����յ��ַ���
//    }
    closesocket(sock);                             // �رռ����׽���
    WSACleanup();                                  // ж��winsock library

    printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
	getchar();


}
