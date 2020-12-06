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
    char temp[1000],temp1[1000]; 
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
       printf("�յ���Ϣ��");
       printf("\n");
	   printf("���ݣ�%s",buffer);
       printf("ʱ�䣺%s",pts);
       send(ssock, buffer, strlen(buffer), 0);
       send(ssock, pts, strlen(pts), 0);
	   printf("�ͻ���IP��ַ��%d.%d.%d.%d\n",fsin.sin_addr.S_un.S_un_b.s_b1,fsin.sin_addr.S_un.S_un_b.s_b2,fsin.sin_addr.S_un.S_un_b.s_b3,fsin.sin_addr.S_un.S_un_b.s_b4);
	   printf("�ͻ��˶˿ںţ�%d\n",fsin.sin_port);
	   num_to_string(fsin.sin_addr.S_un.S_un_b.s_b1,fsin.sin_addr.S_un.S_un_b.s_b2,fsin.sin_addr.S_un.S_un_b.s_b3,fsin.sin_addr.S_un.S_un_b.s_b4,temp);
	   send(ssock,temp,strlen(temp),0); 
	   itoa(fsin.sin_port,temp1,10);
	   send(ssock,temp1,strlen(temp1),0);
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

