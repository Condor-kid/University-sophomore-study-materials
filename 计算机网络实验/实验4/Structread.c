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
	char username[USER_NAME_LEN];      // Ա����
	int level;                         // ���ʼ���
	char email[EMAIL_LEN];             // email��ַ
	DWORD sendtime;                    // ����ʱ��
	time_t regtime;                    // ע��ʱ��
}Person;

int main()
{

	//char pts[TIME_BUF_LEN];              // ʱ���ַ���
	time_t now;                          // ��ǰʱ��

	(void)time(&now);                   // ȡ��ϵͳʱ��
	now=ctime(&now);   // ��ʱ��ת��Ϊ�ַ���
	printf("%s",now);

	struct Person personSent;           // Ҫ���͵�Ա����¼          
	struct Person personRead;
	char textBuf[100];                  // �ı�����
	int  numberBuf;                     // ���ֻ���
	FILE *fp;
	errno_t err;

	int i = 0;
	char buf[sizeof(Person)];
	printf("���Ա����Ϣ\n");
	//����ļ� 
	if ((err = fopen_s(&fp, "Persons.stru", "r")) != 0)
	{
		printf("cannot open file\n");
		exit(0);
	}
	while (fread(&personRead, sizeof(struct Person), 1, fp)!=0)
	{
		char regtime[TIME_BUF_LEN];
		char sendtime[TIME_BUF_LEN];
		printf("Ա��������%s\r\n", personRead.username);
		printf("���ʼ���%d\r\n", personRead.level);
		printf("�����ַ��%s\r\n", personRead.email);
		time_t t1 = (time_t)personRead.sendtime;
		t1=ctime(&t1);
		printf("����ʱ�䣺%s", t1);
		t1=(time_t)personRead.regtime;
		t1=ctime(&t1);
		printf("ע��ʱ�䣺%s", t1);
		printf("\n");
	}
	fclose(fp);
	getchar();
	return 0;
}

