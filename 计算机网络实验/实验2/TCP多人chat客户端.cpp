/* TCPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include<time.h>
#include<process.h>

#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for TIME service
 *------------------------------------------------------------------------
 */
 SOCKET sock,sockets[50]={NULL};//����������50���ͻ��� 
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
			printf("�޷������Ϸ�����\n");
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
	WSAStartup(WSVERS, &wsadata);						  //����winsock library��WSVERSΪ����İ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
  
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //�����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														  //���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

    memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
    sin.sin_family = AF_INET;							  // ��������ַ��
    sin.sin_addr.s_addr = inet_addr(host);                // ���÷�����IP��ַ(32λ)
    sin.sin_port = htons((u_short)atoi(service));         // ���÷������˿ں�  
    int ret=connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // ���ӵ����������ڶ�������ָ���ŷ�������ַ�Ľṹ������������Ϊ�ýṹ�Ĵ�С������ֵΪ0ʱ��ʾ�޴�����������SOCKET_ERROR��ʾ����Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣
 
	printf("�ͻ���������\n");
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
		printf("���ͳɹ�\n\n"); 
	}
	CloseHandle(hThread);
	closesocket(sock);
	WSACleanup();                     /* ж��ĳ�汾��DLL */
	getchar();
	getchar(); 
	//send(sock,buffer,1000,0);//������Ϣ
	//if((buffer[0]=='E'||buffer[0]=='e')&&(buffer[1]='S'||buffer[1]=='s')&&(buffer[2]=='C'||buffer[2]=='c')) 
	//{
	//	printf("�ͻ����ѹر�\n"); 
	//}
	//else
	//{
	//	recv(sock,buffer1,1000,0);
	//	recv(sock,buffer2,1000,0);
	//	recv(sock,buffer3,1000,0);
	//	recv(sock,buffer4,1000,0);
	//	printf("�յ�����Ϣ��");
	//	printf("\n"); 
	//	printf("���ݣ�");
	//	printf("%s",buffer1); 
	//	printf("ʱ�䣺"); 
	//	printf("%s",buffer2); 
	//	printf("�ͻ���IP��ַ��%s",buffer3);
	//	printf("\n");
	//	printf("�ͻ��˶˿ڣ�%s",buffer4);
//	}
//	cc = recv(sock, buf, BUFLEN, 0);                // �ڶ�������ָ�򻺳���������������Ϊ��������С(�ֽ���)�����ĸ�����һ������Ϊ0������ֵ:(>0)���յ����ֽ���,(=0)�Է��ѹر�,(<0)���ӳ���
//  if(cc == SOCKET_ERROR)                          // ����������ر��׽���sock
//     printf("Error: %d.\n",GetLastError());
//     else if(cc == 0) {                             // �Է������ر�
//         printf("Server closed!",buf);  
//    }  else if(cc > 0) {
//         buf[cc] = '\0';	                       // ensure null-termination
//         printf("%s",buf);                         // ��ʾ�����յ��ַ���
//    }
//    closesocket(sock);                             // �رռ����׽���
//   WSACleanup();                                  // ж��winsock library

    printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
	getchar();


}
