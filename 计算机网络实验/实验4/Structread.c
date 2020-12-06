#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUF_LEN 100
#define USER_NAME_LEN 20
#define EMAIL_LEN 80
#define TIME_BUF_LEN 30

typedef unsigned long DWORD;
typedef struct Person {
	char username[USER_NAME_LEN];      // 员工名
	int level;                         // 工资级别
	char email[EMAIL_LEN];             // email地址
	DWORD sendtime;                    // 发送时间
	time_t regtime;                    // 注册时间
}Person;

int main()
{

	//char pts[TIME_BUF_LEN];              // 时间字符串
	time_t now;                          // 当前时间

	(void)time(&now);                   // 取得系统时间
	now=ctime(&now);   // 把时间转换为字符串
	printf("%s",now);

	struct Person personSent;           // 要发送的员工记录          
	struct Person personRead;
	char textBuf[100];                  // 文本缓冲
	int  numberBuf;                     // 数字缓冲
	FILE *fp;
	errno_t err;

	int i = 0;
	char buf[sizeof(Person)];
	printf("输出员工信息\n");
	//输出文件 
	if ((err = fopen_s(&fp, "Persons.stru", "r")) != 0)
	{
		printf("cannot open file\n");
		exit(0);
	}
	while (fread(&personRead, sizeof(struct Person), 1, fp)!=0)
	{
		char regtime[TIME_BUF_LEN];
		char sendtime[TIME_BUF_LEN];
		printf("员工姓名：%s\r\n", personRead.username);
		printf("工资级别：%d\r\n", personRead.level);
		printf("邮箱地址：%s\r\n", personRead.email);
		time_t t1 = (time_t)personRead.sendtime;
		t1=ctime(&t1);
		printf("发送时间：%s", t1);
		t1=(time_t)personRead.regtime;
		t1=ctime(&t1);
		printf("注册时间：%s", t1);
		printf("\n");
	}
	fclose(fp);
	getchar();
	return 0;
}

