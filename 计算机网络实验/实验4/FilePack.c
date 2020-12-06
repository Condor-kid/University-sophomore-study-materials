#include <stdio.h>
#include <stdlib.h>
#include<string.h>
struct FILEStruct   //文件结构 
{
	char filename[301];
	__int64 filesize;
}Node[40];

errno_t err;
int file_size(char* filename)  //文件大小 
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
	
	char destfname[300] = "C:\\Users\\Condor\\Desktop\\FileSet.pak"; //用于目标文件地址 
    printf("打包结束条件为输入exit和回车符\n");
    printf("\n");
	printf("请输入文件名（含地址）：");
	gets(Node[i].filename);
	//Node[i].filesize = file_size(Node[i].filename);



	destfile = fopen(destfname, "wb");   //打开要写入的二进制文件 
	if (destfile==NULL)
	{
		printf("写入文件未找到！\n");
		fclose(destfile);
		exit(1); //中止程序
	}


	while (Node[i].filename[0] != 'e'||Node[i].filename[1]!='x'||Node[i].filename[2]!='i'||Node[i].filename[3]!='t')
	{
		Node[i].filesize = file_size(Node[i].filename);
		srcfile = fopen(Node[i].filename, "rb");      //打开要读取的二进制文件
		if (srcfile==NULL)
		{
			printf("读入文件未找到！\n");
			exit(1); //中止程序
		}

		fwrite(&Node[i], sizeof(struct FILEStruct), 1, destfile);
		int len = 0;
		while ((len = fread(buf, 1, 40, srcfile)) >= 40)
		{   //可以一次读取
			fwrite(buf, 1, 40, destfile);
		}
		fwrite(buf, 1, len, destfile);
		fclose(srcfile);
		i++;
		printf("请输入文件名（含地址）：");
		gets(Node[i].filename);	
	}
	fclose(destfile);
	
	printf("打包结束\n");
	printf("\n");
	getchar();
}

