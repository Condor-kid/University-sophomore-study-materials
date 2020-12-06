#include<stdio.h>
#include<string.h>
#include<WinSock2.h>
#include<process.h>
#include<WS2tcpip.h>
#include<stdbool.h>
#pragma comment(lib,"ws2_32.lib")

#define WSVERS MAKEWORD(2,0)
#define BUFLEN 2000//缓冲区大小

/*
程序有两个线程，一个用于发命令以及接收数据的主线程，另一个用于接收服务器返回来消息(响应码)的线程。
为了同步两个线程，就设置了以下全局变量
*/
bool connected = false;//但connected=true时，收到服务器发来的220，可以发user net
bool passReq = false;//passReq=true时，需要输入密码
bool login = false;//login=true时，用户成功登录，可以设置被动等待命令
bool modeSet = false;//modeSet=true时，说明数据端口已经计算好了
unsigned short dataPort;//被动连接时，服务器给出的端口

//从控制端收到信息
unsigned __stdcall recvFromCtr(void *p)
{
	SOCKET *sock = (SOCKET *)p;//用于连接的socket的指针
	char buf[BUFLEN + 1];
	char ret[4];//响应码
	int rc;
	while (true)//不断接收
	{
		rc = recv(*sock, buf, BUFLEN, 0);
		if (rc > 0)
		{
			buf[rc] = '\0';
			printf("%s", buf);
			strncpy_s(ret, 4, buf, 3);//获取响应码
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
				//获取端口号
				int ip1, ip2, ip3, ip4, port1, port2;
				//把返回信息拆开
				sscanf_s(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n", &ip1, &ip2, &ip3, &ip4, &port1, &port2);
				dataPort = port1 * 256 + port2;//计算端口号
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
	if (argc != 4)//判断命令行参数个数
	{
		printf("Command Error! please input: FtpClient <ip> <sourcePath> <targetPath>\n");
		return -1;
	}
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);//加载winsock library
	const char *host = argv[1];//服务器ip
	const char *controlPort = "21";//控制端口
	struct sockaddr_in ctrIn, dataIn;//控制地址，数据地址
	SOCKET controlSock, dataSock;//控制socket和数据socket
	//初始化
	controlSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	dataSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&ctrIn, 0, sizeof(ctrIn));
	memset(&dataIn, 0, sizeof(dataIn));
	ctrIn.sin_family = AF_INET;
	dataIn.sin_family = AF_INET;
	inet_pton(AF_INET, host, &ctrIn.sin_addr.s_addr);
	inet_pton(AF_INET, host, &dataIn.sin_addr.s_addr);
	ctrIn.sin_port = htons((u_short)atoi(controlPort));
	if (connect(controlSock, (struct sockaddr *)&ctrIn, sizeof(ctrIn)) == SOCKET_ERROR)//连接
	{
		printf("控制端连接服务器失败\n");
		printf("Error: %d\n", GetLastError());
	}
	else
	{
		//创建线程
		HANDLE recvThread = (HANDLE)_beginthreadex(NULL, 0, &recvFromCtr, (void *)&controlSock, 0, NULL);
		const char *user = "user net\r\n";//用户名
		const char *pass = "pass 123456\r\n";//用户密码
		const char *mode = "pasv\r\n";//被动等待
		while (!connected);//等待ftp服务器就绪
		printf("%s", user);
		send(controlSock, user, strlen(user), 0);//输入用户名
		while (!passReq);//等待输入密码
		printf("%s", pass);
		send(controlSock, pass, strlen(pass), 0);//输入密码
		while (!login);//等待登录
		printf("%s", mode);
		send(controlSock, mode, strlen(mode), 0);//输入被动等待
		while (!modeSet);//等待获取端口号
		dataIn.sin_port = htons(dataPort);//甚至数据socket的端口
		if (connect(dataSock, (struct sockaddr *)&dataIn, sizeof(dataIn)) == SOCKET_ERROR)//数据连接
		{
			printf("数据端连接服务器失败\n");
			printf("Error: %d\n", GetLastError());
		}
		else
		{
			FILE *fp = NULL;
			char filePath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT];//文件完整路径
			char fileDrive[_MAX_DRIVE];//驱动器号
			char fileDIR[_MAX_DIR];//中间路径
			char fileName[_MAX_FNAME];//文件名
			char fileExt[_MAX_EXT];//文件扩展名
			_splitpath_s(argv[3], fileDrive, _MAX_DRIVE, fileDIR, _MAX_DIR, fileName, _MAX_FNAME, fileExt, _MAX_EXT);//把输入的文件路径分割
			sprintf_s(filePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, "%s%s%s%s", fileDrive, fileDIR, fileName, fileExt);//重新组合
			int count = 2;
			while (fopen_s(&fp, filePath, "r") == NULL)//判断在目标文件夹里是否已经存在同名的文件
			{
				fclose(fp);
				sprintf_s(filePath, _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT, "%s%s%s(%d)%s", fileDrive, fileDIR, fileName, count, fileExt);
				count++;
			}
			char retr[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 6];
			sprintf_s(retr, 100, "retr %s\r\n", argv[2]);
			printf("%s", retr);
			send(controlSock, retr, strlen(retr), 0);//发送retr命令

			if (fopen_s(&fp, filePath, "wb") != NULL)
			{
				printf("open %s error\n", filePath);
			}
			else
			{
				int rc = 0;
				char buf[BUFLEN];//接收文件缓冲区
				while (true)
				{
					rc = recv(dataSock, buf, BUFLEN, 0);
					if (rc > 0)
					{
						fwrite(buf, sizeof(char), rc, fp);//把接收到的文件写到目标文件里
					}
					else
					{
						break;
					}
				}
			}
			fclose(fp);//关闭文件
			closesocket(dataSock);//关闭socket
		}
		const char *quit = "quit\r\n";//发送退出命令
		printf("%s", quit);
		send(controlSock, quit, strlen(quit), 0);
		WaitForSingleObject(recvThread, INFINITE);
		CloseHandle(recvThread);
		closesocket(controlSock);
	}
	WSACleanup();
	return 0;
}
