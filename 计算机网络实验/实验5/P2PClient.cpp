#include<WinSock2.h>
#include<process.h>
#include<cstdio>
#include<cstring>
#include<sys/stat.h>
#include<WS2tcpip.h>

#define WSVERS MAKEWORD(2,0)
#define BUFLEN 2000
#define MAXINPUTLEN 300//�������һ�ٸ��ֽ�
#define CMDLEN 5 //������4���ַ���һ���ո�

#pragma comment(lib,"ws2_32.lib")


char filePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//�����ļ���·��

//���ṹ
struct packetHead
{
	bool type;//�ļ�(true)��������(false)
};

struct packetFile
{
	char fileName[_MAX_FNAME + _MAX_EXT];
	int length;
};

struct packetChat
{
	int length;
};


SOCKET sock;//һ��sock�͹���

char buf[BUFLEN];
int bufOffset = 0;
int rc;
bool closeConnect = false;


/*ptrָ��Ҫ���յĽṹ�壬lenToRecv�ǽṹ��Ĵ�С*/
int recvStruct(void *p, int lenToRecv)
{
	char *ptr = (char *)p;//���ֽڿ���
	int packetOffset = 0;
	if (bufOffset != 0)
	{
		if (lenToRecv >= rc - bufOffset)
		{
			memcpy_s(ptr, lenToRecv, buf + bufOffset, rc - bufOffset);
			packetOffset = rc - bufOffset;
			bufOffset = 0;
		}
		else
		{
			memcpy_s(ptr, lenToRecv, buf + bufOffset, lenToRecv);
			packetOffset = lenToRecv;
			bufOffset += lenToRecv;
		}
	}
	while (lenToRecv > packetOffset)
	{
		rc = recv(sock, buf, BUFLEN, 0);
		if (rc <= 0)
		{
			return rc;
		}
		if (lenToRecv - packetOffset >= rc)
		{
			memcpy_s(ptr + packetOffset, lenToRecv - packetOffset, buf, rc);
			packetOffset += rc;
		}
		else
		{
			memcpy_s(ptr + packetOffset, lenToRecv - packetOffset, buf, lenToRecv - packetOffset);
			bufOffset = lenToRecv - packetOffset;
			packetOffset = lenToRecv;
		}
	}
	return 1;
}

unsigned __stdcall receive(void *p)
{
	char fileFullPath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];
	char fileBUF[BUFLEN];
	packetHead head;
	packetChat chat;
	packetFile file;
	int dataLength;
	int ret;
	FILE *fp = NULL;//��ʾ������Ϊstdout�����ùرգ����ֵΪNULL����

	while (true)
	{
		memset(&head, 0, sizeof(head));
		memset(&chat, 0, sizeof(chat));
		memset(&file, 0, sizeof(file));
		//����head
		ret = recvStruct(&head, sizeof(head));
		if (ret <= 0)
		{
			break;
		}
		//����chat��file
		if (head.type)//Ϊtrueʱ���ļ����䣬���ļ���ȡ���������ļ���ָ��Ϊfp
		{
			ret = recvStruct(&file, sizeof(file));
			if (ret <= 0)
			{
				break;
			}
			sprintf_s(fileFullPath, "%s%s", filePath, file.fileName);//��ȡ�ļ���
			char fileDrive[_MAX_DRIVE];
			char fileDIR[_MAX_DIR];
			char fileName[_MAX_FNAME];
			char fileExt[_MAX_EXT];
			_splitpath_s(fileFullPath, fileDrive, _MAX_DRIVE, fileDIR, _MAX_DIR, fileName, _MAX_FNAME, fileExt, _MAX_EXT);
			int count = 2;
			while (fopen_s(&fp, fileFullPath, "r") == NULL)
			{
				fclose(fp);
				sprintf_s(fileFullPath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, "%s%s%s(%d)%s", fileDrive, fileDIR, fileName, count, fileExt);
				sprintf_s(file.fileName, _MAX_FNAME + _MAX_EXT, "%s(%d)%s", fileName, count, fileExt);
				count++;
			}
			printf("\r�յ��ļ�: %s\n>", file.fileName);
			if (fopen_s(&fp, fileFullPath, "wb") != NULL)
			{
				printf("open %s failed\n", fileFullPath);
				return 0;
			}
			dataLength = file.length;
		}
		else//����
		{
			ret = recvStruct(&chat, sizeof(chat));
			if (ret <= 0)
			{
				break;
			}
			fp = stdout;
			dataLength = chat.length;
		}
		//����data��������ҪpacketOffset��
		while (dataLength > 0)
		{
			if (BUFLEN >= dataLength)
			{
				ret = recvStruct(fileBUF, dataLength);
				if (ret <= 0)
				{
					break;
				}
				fwrite(fileBUF, 1, dataLength, fp);
				dataLength = 0;
			}
			else
			{
				ret = recvStruct(fileBUF, BUFLEN);
				if (ret <= 0)
				{
					break;
				}
				fwrite(fileBUF, 1, BUFLEN, fp);
				dataLength -= BUFLEN;
			}
		}
		if (head.type)
		{
			fclose(fp);
		}
		else
		{
			fp = NULL;
			printf("\n>");
		}
	}
	closeConnect = true;
	printf("\r���ն������ж�\n");
	return 0;
}

int main()
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);

	const char *host = "127.0.0.1";
	const char *port = "50500";
	struct sockaddr_in sin, fsin;
	int alen;

	char data[MAXINPUTLEN + 1];
	char cmd[CMDLEN + 1];
	char sourceFilePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];
	char sourceDrive[_MAX_DRIVE];
	char sourceDIR[_MAX_DIR];
	char sourceFileName[_MAX_FNAME];
	char sourceExt[_MAX_EXT];
	char sendBUF[BUFLEN];

	packetHead head;
	packetFile file;
	packetChat chat;

	struct stat aFile;
	FILE *fp = NULL;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	inet_pton(AF_INET, host, &sin.sin_addr.s_addr);
	sin.sin_port = htons((u_short)atoi(port));
	printf("��������...\n");
	if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("����ʧ��\n");
		printf("Error: %d\n", GetLastError());
	}
	else
	{
		printf("���ӳɹ�\n");
		HANDLE recvThread = (HANDLE)_beginthreadex(NULL, 0, &receive, NULL, 0, NULL);

		while (true)
		{
			memset(&head, 0, sizeof(head));
			memset(&file, 0, sizeof(file));
			memset(&chat, 0, sizeof(chat));

			printf(">");
			gets_s(data, MAXINPUTLEN + 1);

			if (strcmp(data, "quit") == 0 || closeConnect)
			{
				printf("�˳�\n");
				closesocket(sock);
				break;
			}

			for (int i = 0; i < CMDLEN && i < strlen(data); i++)//��ȡ����
			{
				cmd[i] = data[i];
				cmd[i + 1] = '\0';
			}
			if (strcmp(cmd, "rdir ") == 0)
			{
				memcpy_s(filePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, data + CMDLEN, strlen(data) - CMDLEN + 1);
				int len = strlen(filePath);
				if (filePath[len] != '\\')
				{
					filePath[len] = '\\';
					filePath[len + 1] = '\0';
				}
			}
			else if (strcmp(cmd, "file ") == 0)//�����ļ�
			{
				memcpy_s(sourceFilePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, data + CMDLEN, strlen(data) - CMDLEN + 1);
				if (fopen_s(&fp, sourceFilePath, "rb") != NULL)
				{
					printf("�ļ�������\n>");
				}
				else
				{
					head.type = true;//�ļ�
					
					_splitpath_s(sourceFilePath, sourceDrive, _MAX_DRIVE, sourceDIR, _MAX_DIR, sourceFileName, _MAX_FNAME, sourceExt, _MAX_EXT);

					sprintf_s(file.fileName, _MAX_FNAME + _MAX_EXT, "%s%s", sourceFileName, sourceExt);

					stat(sourceFilePath, &aFile);
					file.length = aFile.st_size;
					send(sock, (char *)&head, sizeof(head), 0);
					send(sock, (char *)&file, sizeof(file), 0);
					int tempLen = file.length;
					while (tempLen >= BUFLEN)
					{
						fread(sendBUF, 1, BUFLEN, fp);
						send(sock, sendBUF, BUFLEN, 0);
						tempLen -= BUFLEN;
					}
					if (tempLen != 0)
					{
						fread(sendBUF, 1, tempLen, fp);
						send(sock, sendBUF, tempLen, 0);
					}
					fclose(fp);
				}
			}
			else//��������
			{
				chat.length = strlen(data);
				memcpy_s(sendBUF, BUFLEN, data, chat.length);
				head.type = false;
				send(sock, (char *)&head, sizeof(head), 0);
				send(sock, (char *)&chat, sizeof(chat), 0);
				send(sock, sendBUF, chat.length, 0);
			}
		}
		CloseHandle(recvThread);
	}
	WSACleanup();
	system("pause");
	return 0;
}