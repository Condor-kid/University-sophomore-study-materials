#include <stdio.h>
#include <string.h>
#include <process.h>
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")
#define BUFLEN 20 << 20 
int finish = 0;
unsigned __stdcall recvMessage(SOCKET *p)
{
	static char buf[BUFLEN];
	while (!finish)
	{
		int c = recv(*p, buf, BUFLEN, 0);
		if (c > 0)
			buf[c] = 0, printf("%s", buf);
		else if (!c)
			printf("Recv connect closed.\n"), finish = 1;
		else if (c == SOCKET_ERROR)
			printf("Recv Error:\n%d\n", GetLastError());
	}
}
int main(int argc, char **argv)
{
	if (argc < 3)
		return 0;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	struct addrinfo *host;
	getaddrinfo(argv[1], argv[2], NULL, &host);
	SOCKET sock = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
	if (connect(sock, host->ai_addr, host->ai_addrlen))
		printf("Connect unsuccessfully.\n");
	HANDLE h = _beginthreadex(NULL, 0, &recvMessage, &sock, 0, NULL);
	static char buf[BUFLEN];
	while (!finish)
	{
		int len = 0;
		for (printf("Input the message(end by EOF, Ctrl+Z in Windows) :\n"); !finish && gets(buf + len);)
			buf[len += strlen(buf + len)] = '\r', buf[++len] = '\n', buf[++len] = 0;
		int c = send(sock, buf, len, 0);
		if (!c)
			printf("Send connect closed.\n"), finish = 1;
		else if (c == SOCKET_ERROR)
			printf("Send Error:\n%d\n", GetLastError());
	}
	CloseHandle(h);
	closesocket(sock);
	freeaddrinfo(host);
	WSACleanup();
	getch();
}



















































