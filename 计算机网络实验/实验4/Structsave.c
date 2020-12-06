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
	printf("Ա������: ");
	gets(textBuf);

	strcpy(personSent.username, textBuf);

	while (textBuf[0] != 'e'||textBuf[1]!='x'||textBuf[2]!='i'||textBuf[3]!='t')
	{
		i++;
		printf("���ʼ���: ");
		scanf_s("%d", &numberBuf, sizeof(int));
		getchar();
		personSent.level = numberBuf;
		printf("�����ַ: ");
		gets(textBuf);
		strcpy(personSent.email, textBuf);
		time_t mynow;
		time(&mynow);
		personSent.sendtime = (DWORD)mynow;
		personSent.regtime = mynow;
		//�����ļ� 
		printf("\n");
		if ((err = fopen_s(&fp, "Persons.stru", "a+")) != 0)
		{
			printf("cannot open file\n");
			exit(0);
		}
		if (fwrite(&personSent, sizeof(struct Person), 1, fp) != 1)
			printf("file write error\n");
		fclose(fp);
		printf("Ա������: ");
		gets(textBuf);
		strcpy(personSent.username, textBuf);
	}
	printf("\npress any key to continue.\n");
	getchar();
	return 0;
}

