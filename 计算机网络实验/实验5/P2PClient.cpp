#include<WinSock2.h>
#include<process.h>
#include<cstdio>
#include<cstring>
#include<sys/stat.h>
#include<WS2tcpip.h>

#define WSVERS MAKEWORD(2,0)
#define BUFLEN 2000
#define MAXINPUTLEN 300//最多输入一百个字节
#define CMDLEN 5 //命令是4个字符加一个空格

#pragma comment(lib,"ws2_32.lib")


char filePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//保存文件的路径

//包结构
struct packetHead
{
	bool type;//文件(true)或者聊天(false)
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


SOCKET sock;//一个sock就够了

char buf[BUFLEN];
int bufOffset = 0;
int rc;
bool closeConnect = false;


/*ptr指向要接收的结构体，lenToRecv是结构体的大小*/
int recvStruct(void *p, int lenToRecv)
{
	char *ptr = (char *)p;//按字节拷贝
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
	FILE *fp = NULL;//显示缓冲区为stdout，不用关闭，最后赋值为NULL即可

	while (true)
	{
		memset(&head, 0, sizeof(head));
		memset(&chat, 0, sizeof(chat));
		memset(&file, 0, sizeof(file));
		//接收head
		ret = recvStruct(&head, sizeof(head));
		if (ret <= 0)
		{
			break;
		}
		//接收chat或file
		if (head.type)//为true时是文件传输，把文件名取出来，打开文件，指针为fp
		{
			ret = recvStruct(&file, sizeof(file));
			if (ret <= 0)
			{
				break;
			}
			sprintf_s(fileFullPath, "%s%s", filePath, file.fileName);//获取文件名
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
			printf("\r收到文件: %s\n>", file.fileName);
			if (fopen_s(&fp, fileFullPath, "wb") != NULL)
			{
				printf("open %s failed\n", fileFullPath);
				return 0;
			}
			dataLength = file.length;
		}
		else//聊天
		{
			ret = recvStruct(&chat, sizeof(chat));
			if (ret <= 0)
			{
				break;
			}
			fp = stdout;
			dataLength = chat.length;
		}
		//接收data，不再需要packetOffset了
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
	printf("\r接收端连接中断\n");
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
	printf("正在连接...\n");
	if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("连接失败\n");
		printf("Error: %d\n", GetLastError());
	}
	else
	{
		printf("连接成功\n");
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
				printf("退出\n");
				closesocket(sock);
				break;
			}

			for (int i = 0; i < CMDLEN && i < strlen(data); i++)//提取命令
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
			else if (strcmp(cmd, "file ") == 0)//发送文件
			{
				memcpy_s(sourceFilePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, data + CMDLEN, strlen(data) - CMDLEN + 1);
				if (fopen_s(&fp, sourceFilePath, "rb") != NULL)
				{
					printf("文件不存在\n>");
				}
				else
				{
					head.type = true;//文件
					
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
			else//聊天数据
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