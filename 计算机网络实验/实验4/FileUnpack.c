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
//	int i = 2;
	
	char destfname[300] = "C:\\Users\\Condor\\Desktop\\FileSet.pak"; //����Ŀ���ļ���ַ 
	//�����ļ� 
	char jieyafile[300];
	printf("���������ļ���");
	gets(jieyafile);
	strcpy(destfname,"C:\\Users\\Condor\\Desktop\\");
	strcat(destfname,jieyafile);
	destfile = fopen(destfname, "rb");   //��Ҫ�����Ķ������ļ� 
	if (destfile==NULL)
	{
		printf("�����ļ�δ�ҵ���\n");
		fclose(destfile);
		exit(1); //��ֹ����
	}
	char *newfilename;
	char *newstr;
	char shuzi[30];
	printf("\n");
	char *find1;
	while (fread(&temp, sizeof(struct FILEStruct), 1, destfile)!=NULL)
	{	
			find1 = strrchr(temp.filename, '\\');
			char jieyanewfile[300];		
			int  len4= strlen(jieyafile);
			strcpy(jieyanewfile, jieyafile);
			newstr = insert(jieyanewfile, find1, len4);			
		    char newstr1[100];
			strcpy(newstr1, newstr);		
		    int cnt=1;
       		while((srcfile = fopen(newstr1, "rb"))!=NULL )
		{
			cnt++;
			if(cnt==2)
			{
			char fu[30] = "()";
			char shuzi[30]={};
			int w=0;
			itoa(cnt, shuzi,10);			
			char *fu1 = insert(fu, shuzi, 1);
			char *find = strrchr(newstr, '.');
	
			   int  len1 = strlen(find);
			   int  len2 = strlen(newstr);
			   int  n = len2 - len1;		       
 			newfilename = insert(newstr1, fu1, n);
 			strcpy(newstr1,newfilename);
			   
			}
			else
			{
				char c=cnt+'0';
				char c1=cnt-1+'0';
				int k=0;
				while(newstr1[k]!=c1)
				{
					k++;
				}
				newstr1[k]=c;
				
			}
					
			fclose(srcfile);
			
		}
		fclose(srcfile);
		printf("%s\n",newstr1);
		srcfile = fopen(newstr1, "wb");   //��Ҫд��Ķ������ļ� 
		if (srcfile==NULL)
		{
			printf("д���ļ�δ�ҵ���\n");
			exit(1); //��ֹ����
		}
		int lenth = temp.filesize;
		char s;
		while (lenth > 0)
		{
			fread(&s, 1, 1, destfile);
			fwrite(&s, 1, 1, srcfile);
			lenth--;
		}

		fclose(srcfile);
	}
	fclose(destfile);
	printf("�������\n");
}

