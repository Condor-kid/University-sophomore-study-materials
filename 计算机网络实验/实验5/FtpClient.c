#include<stdio.h>
#include<string.h>
#include<WinSock2.h>
#include<process.h>
#include<WS2tcpip.h>
#include<stdbool.h>
#pragma comment(lib,"ws2_32.lib")

#define WSVERS MAKEWORD(2,0)
#define BUFLEN 2000//��������С

/*
�����������̣߳�һ�����ڷ������Լ��������ݵ����̣߳���һ�����ڽ��շ�������������Ϣ(��Ӧ��)���̡߳�
Ϊ��ͬ�������̣߳�������������ȫ�ֱ���
*/
bool connected = false;//��connected=trueʱ���յ�������������220�����Է�user net
bool passReq = false;//passReq=trueʱ����Ҫ��������
bool login = false;//login=trueʱ���û��ɹ���¼���������ñ����ȴ�����
bool modeSet = false;//modeSet=trueʱ��˵�����ݶ˿��Ѿ��������
unsigned short dataPort;//��������ʱ�������������Ķ˿�

//�ӿ��ƶ��յ���Ϣ
unsigned __stdcall recvFromCtr(void *p)
{
	SOCKET *sock = (SOCKET *)p;//�������ӵ�socket��ָ��
	char buf[BUFLEN + 1];
	char ret[4];//��Ӧ��
	int rc;
	while (true)//���Ͻ���
	{
		rc = recv(*sock, buf, BUFLEN, 0);
		if (rc > 0)
		{
			buf[rc] = '\0';
			printf("%s", buf);
			strncpy_s(ret, 4, buf, 3);//��ȡ��Ӧ��
			if (strcmp(ret, "220") == 0)//Microsoft FTP Service
			{
				connected = true;
			}
			else if (strcmp(ret, "331") == 0)//Password required
			{
				passReq = true;
			}
			else if (strcmp(ret, "230") == 0)//User logged in
			{
				login = true;
			}
			else if (strcmp(ret, "227") == 0)//Entering Passive Mode
			{
				//��ȡ�˿ں�
				int ip1, ip2, ip3, ip4, port1, port2;
				//�ѷ�����Ϣ��
				sscanf_s(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n", &ip1, &ip2, &ip3, &ip4, &port1, &port2);
				dataPort = port1 * 256 + port2;//����˿ں�
				modeSet = true;
			}
		}
		else
		{
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 4)//�ж������в�������
	{
		printf("Command Error! please input: FtpClient <ip> <sourcePath> <targetPath>\n");
		return -1;
	}
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);//����winsock library
	const char *host = argv[1];//������ip
	const char *controlPort = "21";//���ƶ˿�
	struct sockaddr_in ctrIn, dataIn;//���Ƶ�ַ�����ݵ�ַ
	SOCKET controlSock, dataSock;//����socket������socket
	//��ʼ��
	controlSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	dataSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&ctrIn, 0, sizeof(ctrIn));
	memset(&dataIn, 0, sizeof(dataIn));
	ctrIn.sin_family = AF_INET;
	dataIn.sin_family = AF_INET;
	inet_pton(AF_INET, host, &ctrIn.sin_addr.s_addr);
	inet_pton(AF_INET, host, &dataIn.sin_addr.s_addr);
	ctrIn.sin_port = htons((u_short)atoi(controlPort));
	if (connect(controlSock, (struct sockaddr *)&ctrIn, sizeof(ctrIn)) == SOCKET_ERROR)//����
	{
		printf("���ƶ����ӷ�����ʧ��\n");
		printf("Error: %d\n", GetLastError());
	}
	else
	{
		//�����߳�
		HANDLE recvThread = (HANDLE)_beginthreadex(NULL, 0, &recvFromCtr, (void *)&controlSock, 0, NULL);
		const char *user = "user net\r\n";//�û���
		const char *pass = "pass 123456\r\n";//�û�����
		const char *mode = "pasv\r\n";//�����ȴ�
		while (!connected);//�ȴ�ftp����������
		printf("%s", user);
		send(controlSock, user, strlen(user), 0);//�����û���
		while (!passReq);//�ȴ���������
		printf("%s", pass);
		send(controlSock, pass, strlen(pass), 0);//��������
		while (!login);//�ȴ���¼
		printf("%s", mode);
		send(controlSock, mode, strlen(mode), 0);//���뱻���ȴ�
		while (!modeSet);//�ȴ���ȡ�˿ں�
		dataIn.sin_port = htons(dataPort);//��������socket�Ķ˿�
		if (connect(dataSock, (struct sockaddr *)&dataIn, sizeof(dataIn)) == SOCKET_ERROR)//��������
		{
			printf("���ݶ����ӷ�����ʧ��\n");
			printf("Error: %d\n", GetLastError());
		}
		else
		{
			FILE *fp = NULL;
			char filePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//�ļ�����·��
			char fileDrive[_MAX_DRIVE];//��������
			char fileDIR[_MAX_DIR];//�м�·��
			char fileName[_MAX_FNAME];//�ļ���
			char fileExt[_MAX_EXT];//�ļ���չ��
			_splitpath_s(argv[3], fileDrive, _MAX_DRIVE, fileDIR, _MAX_DIR, fileName, _MAX_FNAME, fileExt, _MAX_EXT);//��������ļ�·���ָ�
			sprintf_s(filePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, "%s%s%s%s", fileDrive, fileDIR, fileName, fileExt);//�������
			int count = 2;
			while (fopen_s(&fp, filePath, "r") == NULL)//�ж���Ŀ���ļ������Ƿ��Ѿ�����ͬ�����ļ�
			{
				fclose(fp);
				sprintf_s(filePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, "%s%s%s(%d)%s", fileDrive, fileDIR, fileName, count, fileExt);
				count++;
			}
			char retr[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 6];
			sprintf_s(retr, 100, "retr %s\r\n", argv[2]);
			printf("%s", retr);
			send(controlSock, retr, strlen(retr), 0);//����retr����

			if (fopen_s(&fp, filePath, "wb") != NULL)
			{
				printf("open %s error\n", filePath);
			}
			else
			{
				int rc = 0;
				char buf[BUFLEN];//�����ļ�������
				while (true)
				{
					rc = recv(dataSock, buf, BUFLEN, 0);
					if (rc > 0)
					{
						fwrite(buf, sizeof(char), rc, fp);//�ѽ��յ����ļ�д��Ŀ���ļ���
					}
					else
					{
						break;
					}
				}
			}
			fclose(fp);//�ر��ļ�
			closesocket(dataSock);//�ر�socket
		}
		const char *quit = "quit\r\n";//�����˳�����
		printf("%s", quit);
		send(controlSock, quit, strlen(quit), 0);
		WaitForSingleObject(recvThread, INFINITE);
		CloseHandle(recvThread);
		closesocket(controlSock);
	}
	WSACleanup();
	return 0;
}
