#include<WinSock2.h>
#include<process.h>
#include<cstdio>
#include<cstring>
#include<sys/stat.h>

#define WSVERS MAKEWORD(2,0)
#define BUFLEN 2000//缓冲区大小
#define MAXINPUTLEN 300//最多输入一百个字节
#define CMDLEN 5 //命令是4个字符加一个空格

#pragma comment(lib,"ws2_32.lib")


char filePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//保存文件的路径

//包结构
//指明类型，时文件还是聊天
struct packetHead
{
	bool type;//文件(true)或者聊天(false)
};
//文件包结构为文件名+长度
struct packetFile
{
	char fileName[_MAX_FNAME + _MAX_EXT];
	int length;
};
//聊天包结构为长度
struct packetChat
{
	int length;
};


SOCKET ssock;//用于连接的套接字

char buf[BUFLEN];//缓冲区
int bufOffset = 0;//缓冲区偏移量，bufOffset开始是还没有被读走的数据，bufOffset==0是，表示已经全被读走
int rc;//收到的数据的字节数
bool closeConnect = false;


/*ptr指向要接收的结构体也可以指向缓冲区，lenToRecv是要接收多少个字节*/
int recvStruct(void *p, int lenToRecv)
{
	char *ptr = (char *)p;//按字节拷贝
	int packetOffset = 0;//已经收到的结构体的字节数
	//bufOffset不等于0，说明接收缓冲区里有一些字节还没有被读走，要把剩余的字节取出来，才能接收新的数据
	if (bufOffset != 0)
	{
		if (lenToRecv >= rc - bufOffset)//如果结构体的大小>=剩余的字节，就把缓冲区里的字节全读走
		{
			memcpy_s(ptr, lenToRecv, buf + bufOffset, rc - bufOffset);
			packetOffset = rc - bufOffset;
			bufOffset = 0;//此时接收缓冲区里的字节全被读走，bufOffset=0
		}
		else//结构体大小比剩余字节少，就读一部分
		{
			memcpy_s(ptr, lenToRecv, buf + bufOffset, lenToRecv);
			packetOffset = lenToRecv;
			bufOffset += lenToRecv;
		}
	}
	while (lenToRecv > packetOffset)//判断结构体是否已经完整
	{
		rc = recv(ssock, buf, BUFLEN, 0);
		if (rc <= 0)
		{
			return rc;
		}
		if (lenToRecv - packetOffset >= rc)//如果结构体的大小>=剩余的字节，就把缓冲区里的字节全读走
		{
			memcpy_s(ptr + packetOffset, lenToRecv - packetOffset, buf, rc);
			packetOffset += rc;
		}
		else//结构体大小比剩余字节少，就读一部分
		{
			memcpy_s(ptr + packetOffset, lenToRecv - packetOffset, buf, lenToRecv - packetOffset);
			bufOffset = lenToRecv - packetOffset;
			packetOffset = lenToRecv;
		}
	}
	return 1;//成功接收返回1
}

//接收线程
unsigned __stdcall receive(void *p)
{
	char fileFullPath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//文件的完整路径
	char fileBUF[BUFLEN];//文件接收缓冲区
	packetHead head;//头结构体
	packetChat chat;//聊天包
	packetFile file;//文件包
	int dataLength;//要接收的数据部分的长度
	int ret;
	FILE *fp = NULL;//指向文件或聊天数据部分要写到目标文件，显示缓冲区为stdout，不用关闭，最后赋值为NULL即可，

	while (true)//每次循环，就完成一次完整的接收，即结构体1+结构体2+数据
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
			sprintf_s(fileFullPath, "%s%s", filePath, file.fileName);//获取文件的完整路径
			char fileDrive[_MAX_DRIVE];
			char fileDIR[_MAX_DIR];
			char fileName[_MAX_FNAME];
			char fileExt[_MAX_EXT];
			_splitpath_s(fileFullPath, fileDrive, _MAX_DRIVE, fileDIR, _MAX_DIR, fileName, _MAX_FNAME, fileExt, _MAX_EXT);
			int count = 2;
			while (fopen_s(&fp, fileFullPath, "r") == NULL)//判断是否存在重名的文件
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
			fp = stdout;//把消息写到输出缓冲区stdout，而不是具体的文件
			dataLength = chat.length;
		}
		//接收data，不再需要packetOffset了
		while (dataLength > 0)//要接收的数据的字节数
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
		//关闭文件指针
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
	//退出循环意味着连接中断
	printf("\r接收端连接中断\n");
	closeConnect = true;
	return 0;
}

int main()
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);

	const char *port = "50500";//端口是50500
	struct sockaddr_in sin, fsin;
	int alen;
	SOCKET msock;//监听套接字

	char data[MAXINPUTLEN + 1];//
	char cmd[CMDLEN + 1];//命令有rdir, send, quit
	char sourceFilePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//要发送的文件的完整路径
	char sourceDrive[_MAX_DRIVE];//要发送的文件的驱动器号
	char sourceDIR[_MAX_DIR];//要发送的文件的中间路径
	char sourceFileName[_MAX_FNAME];//要发送的文件的文件名
	char sourceExt[_MAX_EXT];//要发送的文件的文件后缀
	char sendBUF[BUFLEN];//发送缓冲区

	//结构体
	packetHead head;
	packetFile file;
	packetChat chat;

	//用于获取文件大小
	struct stat aFile;
	FILE *fp = NULL;

	//创建监听套接字
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((u_short)atoi(port));
	if (bind(msock, (struct sockaddr *)&sin, sizeof(sin)) == 0)//绑定端口
	{
		listen(msock, 1);//点到点，队列为1
		printf("等待连接...\n");
		alen = sizeof(struct sockaddr);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);//创建连接套接字
		printf("连接成功\n");
		//创建接收线程
		HANDLE recvThread = (HANDLE)_beginthreadex(NULL, 0, &receive, NULL, 0, NULL);

		while (true)//循环发送
		{
			memset(&head, 0, sizeof(head));
			memset(&file, 0, sizeof(file));
			memset(&chat, 0, sizeof(chat));

			printf(">");
			gets_s(data, MAXINPUTLEN + 1);//输入

			if (closeConnect || strcmp(data, "quit") == 0)
			{
				printf("退出\n");
				closesocket(msock);
				closesocket(ssock);
				break;
			}

			for (int i = 0; i < CMDLEN && i < strlen(data); i++)//提取命令
			{
				cmd[i] = data[i];
				cmd[i + 1] = '\0';
			}
			if (strcmp(cmd, "rdir ") == 0)//设定接收文件夹
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

					sprintf_s(file.fileName, _MAX_FNAME + _MAX_EXT, "%s%s", sourceFileName, sourceExt);//获取文件名

					stat(sourceFilePath, &aFile);//获取文件大小
					file.length = aFile.st_size;
					send(ssock, (char *)&head, sizeof(head), 0);
					send(ssock, (char *)&file, sizeof(file), 0);
					int tempLen = file.length;
					while (tempLen >= BUFLEN)//发送文件内容
					{
						fread(sendBUF, 1, BUFLEN, fp);
						send(ssock, sendBUF, BUFLEN, 0);
						tempLen -= BUFLEN;
					}
					if (tempLen != 0)
					{
						fread(sendBUF, 1, tempLen, fp);
						send(ssock, sendBUF, tempLen, 0);
					}
					fclose(fp);
				}
			}
			else//聊天数据
			{
				chat.length = strlen(data);//数据长度
				memcpy_s(sendBUF, BUFLEN, data, chat.length);//拷贝到发送缓冲区
				head.type = false;
				send(ssock, (char *)&head, sizeof(head), 0);
				send(ssock, (char *)&chat, sizeof(chat), 0);
				send(ssock, sendBUF, chat.length, 0);
			}
		}
		CloseHandle(recvThread);
	}
	WSACleanup();
	system("pause");
	return 0;
}