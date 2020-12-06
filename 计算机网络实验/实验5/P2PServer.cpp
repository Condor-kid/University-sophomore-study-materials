#include<WinSock2.h>
#include<process.h>
#include<cstdio>
#include<cstring>
#include<sys/stat.h>

#define WSVERS MAKEWORD(2,0)
#define BUFLEN 2000//��������С
#define MAXINPUTLEN 300//�������һ�ٸ��ֽ�
#define CMDLEN 5 //������4���ַ���һ���ո�

#pragma comment(lib,"ws2_32.lib")


char filePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//�����ļ���·��

//���ṹ
//ָ�����ͣ�ʱ�ļ���������
struct packetHead
{
	bool type;//�ļ�(true)��������(false)
};
//�ļ����ṹΪ�ļ���+����
struct packetFile
{
	char fileName[_MAX_FNAME + _MAX_EXT];
	int length;
};
//������ṹΪ����
struct packetChat
{
	int length;
};


SOCKET ssock;//�������ӵ��׽���

char buf[BUFLEN];//������
int bufOffset = 0;//������ƫ������bufOffset��ʼ�ǻ�û�б����ߵ����ݣ�bufOffset==0�ǣ���ʾ�Ѿ�ȫ������
int rc;//�յ������ݵ��ֽ���
bool closeConnect = false;


/*ptrָ��Ҫ���յĽṹ��Ҳ����ָ�򻺳�����lenToRecv��Ҫ���ն��ٸ��ֽ�*/
int recvStruct(void *p, int lenToRecv)
{
	char *ptr = (char *)p;//���ֽڿ���
	int packetOffset = 0;//�Ѿ��յ��Ľṹ����ֽ���
	//bufOffset������0��˵�����ջ���������һЩ�ֽڻ�û�б����ߣ�Ҫ��ʣ����ֽ�ȡ���������ܽ����µ�����
	if (bufOffset != 0)
	{
		if (lenToRecv >= rc - bufOffset)//����ṹ��Ĵ�С>=ʣ����ֽڣ��Ͱѻ���������ֽ�ȫ����
		{
			memcpy_s(ptr, lenToRecv, buf + bufOffset, rc - bufOffset);
			packetOffset = rc - bufOffset;
			bufOffset = 0;//��ʱ���ջ���������ֽ�ȫ�����ߣ�bufOffset=0
		}
		else//�ṹ���С��ʣ���ֽ��٣��Ͷ�һ����
		{
			memcpy_s(ptr, lenToRecv, buf + bufOffset, lenToRecv);
			packetOffset = lenToRecv;
			bufOffset += lenToRecv;
		}
	}
	while (lenToRecv > packetOffset)//�жϽṹ���Ƿ��Ѿ�����
	{
		rc = recv(ssock, buf, BUFLEN, 0);
		if (rc <= 0)
		{
			return rc;
		}
		if (lenToRecv - packetOffset >= rc)//����ṹ��Ĵ�С>=ʣ����ֽڣ��Ͱѻ���������ֽ�ȫ����
		{
			memcpy_s(ptr + packetOffset, lenToRecv - packetOffset, buf, rc);
			packetOffset += rc;
		}
		else//�ṹ���С��ʣ���ֽ��٣��Ͷ�һ����
		{
			memcpy_s(ptr + packetOffset, lenToRecv - packetOffset, buf, lenToRecv - packetOffset);
			bufOffset = lenToRecv - packetOffset;
			packetOffset = lenToRecv;
		}
	}
	return 1;//�ɹ����շ���1
}

//�����߳�
unsigned __stdcall receive(void *p)
{
	char fileFullPath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//�ļ�������·��
	char fileBUF[BUFLEN];//�ļ����ջ�����
	packetHead head;//ͷ�ṹ��
	packetChat chat;//�����
	packetFile file;//�ļ���
	int dataLength;//Ҫ���յ����ݲ��ֵĳ���
	int ret;
	FILE *fp = NULL;//ָ���ļ����������ݲ���Ҫд��Ŀ���ļ�����ʾ������Ϊstdout�����ùرգ����ֵΪNULL���ɣ�

	while (true)//ÿ��ѭ���������һ�������Ľ��գ����ṹ��1+�ṹ��2+����
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
			sprintf_s(fileFullPath, "%s%s", filePath, file.fileName);//��ȡ�ļ�������·��
			char fileDrive[_MAX_DRIVE];
			char fileDIR[_MAX_DIR];
			char fileName[_MAX_FNAME];
			char fileExt[_MAX_EXT];
			_splitpath_s(fileFullPath, fileDrive, _MAX_DRIVE, fileDIR, _MAX_DIR, fileName, _MAX_FNAME, fileExt, _MAX_EXT);
			int count = 2;
			while (fopen_s(&fp, fileFullPath, "r") == NULL)//�ж��Ƿ�����������ļ�
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
			fp = stdout;//����Ϣд�����������stdout�������Ǿ�����ļ�
			dataLength = chat.length;
		}
		//����data��������ҪpacketOffset��
		while (dataLength > 0)//Ҫ���յ����ݵ��ֽ���
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
		//�ر��ļ�ָ��
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
	//�˳�ѭ����ζ�������ж�
	printf("\r���ն������ж�\n");
	closeConnect = true;
	return 0;
}

int main()
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);

	const char *port = "50500";//�˿���50500
	struct sockaddr_in sin, fsin;
	int alen;
	SOCKET msock;//�����׽���

	char data[MAXINPUTLEN + 1];//
	char cmd[CMDLEN + 1];//������rdir, send, quit
	char sourceFilePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//Ҫ���͵��ļ�������·��
	char sourceDrive[_MAX_DRIVE];//Ҫ���͵��ļ�����������
	char sourceDIR[_MAX_DIR];//Ҫ���͵��ļ����м�·��
	char sourceFileName[_MAX_FNAME];//Ҫ���͵��ļ����ļ���
	char sourceExt[_MAX_EXT];//Ҫ���͵��ļ����ļ���׺
	char sendBUF[BUFLEN];//���ͻ�����

	//�ṹ��
	packetHead head;
	packetFile file;
	packetChat chat;

	//���ڻ�ȡ�ļ���С
	struct stat aFile;
	FILE *fp = NULL;

	//���������׽���
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((u_short)atoi(port));
	if (bind(msock, (struct sockaddr *)&sin, sizeof(sin)) == 0)//�󶨶˿�
	{
		listen(msock, 1);//�㵽�㣬����Ϊ1
		printf("�ȴ�����...\n");
		alen = sizeof(struct sockaddr);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);//���������׽���
		printf("���ӳɹ�\n");
		//���������߳�
		HANDLE recvThread = (HANDLE)_beginthreadex(NULL, 0, &receive, NULL, 0, NULL);

		while (true)//ѭ������
		{
			memset(&head, 0, sizeof(head));
			memset(&file, 0, sizeof(file));
			memset(&chat, 0, sizeof(chat));

			printf(">");
			gets_s(data, MAXINPUTLEN + 1);//����

			if (closeConnect || strcmp(data, "quit") == 0)
			{
				printf("�˳�\n");
				closesocket(msock);
				closesocket(ssock);
				break;
			}

			for (int i = 0; i < CMDLEN && i < strlen(data); i++)//��ȡ����
			{
				cmd[i] = data[i];
				cmd[i + 1] = '\0';
			}
			if (strcmp(cmd, "rdir ") == 0)//�趨�����ļ���
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

					sprintf_s(file.fileName, _MAX_FNAME + _MAX_EXT, "%s%s", sourceFileName, sourceExt);//��ȡ�ļ���

					stat(sourceFilePath, &aFile);//��ȡ�ļ���С
					file.length = aFile.st_size;
					send(ssock, (char *)&head, sizeof(head), 0);
					send(ssock, (char *)&file, sizeof(file), 0);
					int tempLen = file.length;
					while (tempLen >= BUFLEN)//�����ļ�����
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
			else//��������
			{
				chat.length = strlen(data);//���ݳ���
				memcpy_s(sendBUF, BUFLEN, data, chat.length);//���������ͻ�����
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