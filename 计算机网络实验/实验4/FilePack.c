#include <stdio.h>
#include <stdlib.h>
#include<string.h>
struct FILEStruct   //�ļ��ṹ 
{
	char filename[301];
	__int64 filesize;
}Node[40];

errno_t err;
int file_size(char* filename)  //�ļ���С 
{
	FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  

	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fclose(fp);

	return size;
}

char *insert(char *s1, char *s2, int n)
{
	int len1 = 0, len2 = 0, j = 0, k = 0;
	char s4[30];
	char *s3 = s4;
	if (s1 == NULL)
		return NULL;
	if (s2 == NULL)
		return s1;
	len1 = strlen(s1);
	len2 = strlen(s2);

	if (n > len1)
		return NULL;
	int i;
	for (i = 0; i < n; i++)
	{
		j++;
	}
	for (i = 0; i < len1; i++)
	{
		s4[k++] = s1[i];
	}

	for (i = 0; i < len2; i++)
		s1[j++] = s2[i];

	for (i = n; i < len1; i++)
		s1[j++] = s4[i];

	s1[j] = '\0';
	
	return s1;
}

struct FILEStruct temp;
int main()
{
	FILE *srcfile;
	FILE *destfile;
	
	char buf[40];
	int i = 0;
	
	char destfname[300] = "C:\\Users\\Condor\\Desktop\\FileSet.pak"; //����Ŀ���ļ���ַ 
    printf("�����������Ϊ����exit�ͻس���\n");
    printf("\n");
	printf("�������ļ���������ַ����");
	gets(Node[i].filename);
	//Node[i].filesize = file_size(Node[i].filename);



	destfile = fopen(destfname, "wb");   //��Ҫд��Ķ������ļ� 
	if (destfile==NULL)
	{
		printf("д���ļ�δ�ҵ���\n");
		fclose(destfile);
		exit(1); //��ֹ����
	}


	while (Node[i].filename[0] != 'e'||Node[i].filename[1]!='x'||Node[i].filename[2]!='i'||Node[i].filename[3]!='t')
	{
		Node[i].filesize = file_size(Node[i].filename);
		srcfile = fopen(Node[i].filename, "rb");      //��Ҫ��ȡ�Ķ������ļ�
		if (srcfile==NULL)
		{
			printf("�����ļ�δ�ҵ���\n");
			exit(1); //��ֹ����
		}

		fwrite(&Node[i], sizeof(struct FILEStruct), 1, destfile);
		int len = 0;
		while ((len = fread(buf, 1, 40, srcfile)) >= 40)
		{   //����һ�ζ�ȡ
			fwrite(buf, 1, 40, destfile);
		}
		fwrite(buf, 1, len, destfile);
		fclose(srcfile);
		i++;
		printf("�������ļ���������ַ����");
		gets(Node[i].filename);	
	}
	fclose(destfile);
	
	printf("�������\n");
	printf("\n");
	getchar();
}

