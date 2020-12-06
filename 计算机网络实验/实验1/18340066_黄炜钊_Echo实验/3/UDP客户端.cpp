/* UDPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>

#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 2)        // ָ���汾2.2 
#pragma comment(lib,"ws2_32.lib")         // ����winsock 2.2 Llibrary

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
    WSAStartup(WSVERS, &wsadata);       /* ����ĳ�汾Socket��DLL        */	

    sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP);

	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons((u_short)atoi(service));    //atoi����asciiת��Ϊint. htons��������(host)ת��Ϊ������(network), s--short
	toAddr.sin_addr.s_addr = inet_addr(host);           //���hostΪ��������Ҫ���ú���gethostbyname������ת��ΪIP��ַ


//	memset(buf,'e',BUFLEN);  //fill 1000 bytes with 'e'
//	buf[BUFLEN] = '\0';
    (void) time(&now);                                      // ȡ��ϵͳʱ��
    pts = ctime(&now);                                      // ��ʱ��ת��Ϊ�ַ���
    //memcpy(buf,pts,strlen(pts));
    printf("ʱ�䣺");
    printf("%s",pts);
    printf("������Ϣ��");
	scanf("%s",buffer1); 
	sendto(sock,buffer1,BUFLEN,0,(SOCKADDR *)&toAddr, sizeof(toAddr));
    sendto(sock,pts, BUFLEN, 0,(SOCKADDR *)&toAddr, sizeof(toAddr));
    printf("�յ�����Ϣ��");
    printf("%s",buffer1);
    printf("\n"); 
    recvfrom(sock,buffer2,BUFLEN,0,(SOCKADDR *)&toAddr, &mysize);
    recvfrom(sock,buffer3,BUFLEN,0,(SOCKADDR *)&toAddr, &mysize);
    printf("�ͻ��˵�ַ��");
    printf("%s",buffer2);
    printf("\n");
    printf("�ͻ��˶˿ڣ�");
    printf("%s",buffer3);
    printf("\n");
   // if (cc == SOCKET_ERROR){
   //     printf("����ʧ�ܣ�����ţ�%d\n", WSAGetLastError());
   //  }
   //else{
   //		printf("���ͳɹ�!\r\n");
   //}

	closesocket(sock);
	WSACleanup();       	          /* ж��ĳ�汾��DLL */  

	printf("��������˳�...");
	getchar();
	getchar(); 

}
