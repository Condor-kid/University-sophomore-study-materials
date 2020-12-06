/* TCPServer.cpp - main */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "conio.h"

#define	WSVERS	MAKEWORD(2, 0)

#pragma comment(lib,"ws2_32.lib")  //ʹ��winsock 2.2 library
/*------------------------------------------------------------------------
 * main - Iterative TCP server for TIME service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) 
/* argc: �����в��������� ���磺C:\> TCPServer 8080 
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
    char buffer[1000];// ������ 

	if(WSAStartup(WSVERS, &wsadata)==SOCKET_ERROR)// ����winsock library��WSVERSָ������ʹ�õİ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
	{
		printf("����������ʧ�ܣ�\n");
	}
	else
	{
		printf("��������������\n");
	}						
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// �����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														// ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	sin.sin_family = AF_INET;							// ��������ַ��(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// ��������(�ӿڵ�)IP��ַ��
	sin.sin_port = htons((u_short)atoi(service));		// �����Ķ˿ںš�atoi--��asciiת��Ϊint��htons--������������(host to network��s-short 16λ)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // �󶨼�����IP��ַ�Ͷ˿ں�

	listen(msock, 5);                                   // ��������Ϊ5������������У����ѵ������������������еȴ�����

    while(1){ 		                                   // ����Ƿ��а���,���û�������ѭ����ִ��
       alen = sizeof(struct sockaddr);                         // ȡ����ַ�ṹ�ĳ���
	   ssock = accept(msock, (struct sockaddr *)&fsin, &alen); // ����������������������������������������󲢽������ӣ����ظ����ӵ��׽��֣����򣬱���䱻����ֱ�����зǿա�fsin�����ͻ���IP��ַ�Ͷ˿ں�
	    
       memset(buffer,0,sizeof(buffer));//��ʼ�� 
	   recv(ssock,buffer,1000,0);//���տͻ�������
	   if((buffer[0]=='E'||buffer[0]=='e')&&(buffer[1]='S'||buffer[1]=='s')&&(buffer[2]=='C'||buffer[2]=='c')) 
	   //����esc�˳� 
	   {
	   	  printf("�������ѹر�\n");
	   	  break;
	   } 
	   (void) time(&now);                                      // ȡ��ϵͳʱ��
       pts = ctime(&now);                                      // ��ʱ��ת��Ϊ�ַ���
       printf("�յ���Ϣ��%s",buffer);
       printf("�յ�ʱ�䣺%s",pts);
       send(ssock, buffer, strlen(buffer), 0);
       send(ssock, pts, strlen(pts), 0);
//	   int cc = send(ssock, pts, strlen(pts), 0);              // �ڶ�������ָ���ͻ�����������������ΪҪ���͵��ֽ��������ĸ�����һ����0������ֵ��>=0 ʵ�ʷ��͵��ֽ�����0 �Է������رգ�SOCKET_ERROR ����
//       printf("%s", pts);                                      // ��ʾ���͵��ַ���
       (void) closesocket(ssock);                              // �ر������׽���
     }
    (void) closesocket(msock);                                 // �رռ����׽���
     WSACleanup();                                             // ж��winsock library
     printf("���س�������...");
 	 getchar();										// �ȴ����ⰴ��
	 getchar();


}

