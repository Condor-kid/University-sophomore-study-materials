/* UDPServer.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "conio.h"


#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 2)        // ָ���汾2.2 
#pragma comment(lib,"ws2_32.lib")         // ����winsock 2.2 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for DAYTIME service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	char   *host = "127.0.0.1";	    /* server IP Address to connect */
	char   *service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	        /* an Internet endpoint address	*/
	struct sockaddr_in from;        /* sender address               */
	int    fromsize = sizeof(from);
	char   buf[BUFLEN+1];   	    /* buffer for one line of text	*/
	SOCKET	sock;		  	        /* socket descriptor	    	*/
	int	cc;			                /* recv character count		    */
    char temp[BUFLEN+1];
    
	WSADATA wsadata;
    WSAStartup(WSVERS, &wsadata);   /* ����winsock library��WSVERSΪ����汾,wsadata����ϵͳʵ��֧�ֵ���߰汾��    */		
    sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP); // ����UDP�׽���, ������������Э���(family)�����ݱ��׽��֣�UDPЭ��ţ� ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;                     // ��(����)���еĽӿڡ�
	sin.sin_port = htons((u_short)atoi(service));         // ��ָ���ӿڡ�atoi--��asciiת��Ϊint��htons -- ������(host)ת��Ϊ������(network), Ϊshort���͡� 
	bind(sock, (struct sockaddr *)&sin, sizeof(sin));     // �󶨱��ض˿ںţ��ͱ���IP��ַ)

 	while(!_kbhit()){                                    //����Ƿ��а���
	    cc = recvfrom(sock, buf, BUFLEN, 0, (SOCKADDR *)&from, &fromsize);  //���տͻ����ݡ����ؽ����ccΪ���յ��ַ�����from�н������ͻ�IP��ַ�Ͷ˿ںš�
        printf("�ͻ��˵���Ϣ��");
        printf("%s",buf);
        printf("\n"); 
        recvfrom(sock,buf,BUFLEN,0,(SOCKADDR *)&from, &fromsize);
        printf("ʱ�䣺");
		printf("%s",buf);
		printf("�ͻ��˵�IP��ַ��%s\n",inet_ntoa(from.sin_addr));
		printf("�ͻ��˶˿ںţ�%d\n",from.sin_port); 
		sendto(sock,inet_ntoa(from.sin_addr),BUFLEN,0,(SOCKADDR *)&from,sizeof(from));
		itoa(from.sin_port,temp,10);
		sendto(sock,temp,BUFLEN,0,(SOCKADDR *)&from,sizeof(from));
	  //  if (cc == SOCKET_ERROR){
      //      printf("recvfrom() failed; %d\n", WSAGetLastError());
      //      break;
      //  }
      // else if (cc == 0)
      //     break;
	  //	else{
	  //	buf[cc] = '\0';
	  //	printf("%s\n", buf);
	  //}
	}
	closesocket(sock);
	WSACleanup();       	          /* ж��ĳ�汾��DLL */
	getchar();
	getchar();
}



