#include<stdio.h>
#define N 300
int A[N][N];
int B[N][N];
int **C;

typedef struct{
    int line,column;
}kk;

void div_matrix(kk a_front,kk a_last,kk b_front,kk b_last);
int main()
{
    kk a_front,a_last,b_front,b_last;
    a_front.line=a_front.column=b_front.line=b_front.column=0;
    a_last.column=a_last.line=b_last.line=b_last.column=N;
    int i,j=0;
    C=(int **)malloc(N,sizeof(int *));
    for(i=0;i<N;i++)
    {
        C[i]=(int *)malloc(N,sizeof(int));
    }
    //初始化
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            A[i][j]=B[i][j]=2;
        }
    }
    div_matrix(a_front,a_last,b_front,b_last);
    return 0;
}
void div_matrix(kk a_front,kk a_last,kk b_front,kk b_last)
{
    if(a_front.line==a_last.line&&b_front.column==b_last.column)
    {
        C[a_front.line][b_front.column]+=A[a_front.line][a_front.column]*B[b_front.line][b_front.column];
        return;
    }
    kk a000,a001,a010,a011,a100,a101,a110,a111;
    kk b000,b001,b010,b011,b100,b101,b110,b111;
    //每块矩阵均分成四块进行递归
    a000.line=a_front.line;
    a000.column=a_front.column;
    a001.line=(a_front.line+a_last.line)/2;
    a001.column=(a_front.column+a_last.column)/2;
    //1
    a010.line=a_front.line;
    a010.column=(a_front.column+a_last.column)/2+1;
    a011.line=(a_front.line+a_last.line)/2;
    a011.column=a_last.column;
    //2
    a100.line=(a_front.line+a_last.line)/2+1;
    a100.column=a_front.column;
    a101.line=a_last.line;
    a101.column=(a_front.column+a_last.column)/2;
    //3
    a110.line=(a_front.line+a_last.line)/2+1;
    a110.column=(a_front.column+a_last.column)/2+1;
    a111.line=a_last.line;
    a111.column=a_last.column;
    //4
    b000.line=b_front.line;
    b000.column=b_front.column;
    b001.line=(b_front.line+b_last.line)/2;
    b001.column=(b_front.column+b_last.column)/2;
    //5
    b010.line=b_front.line;
    b010.column=(b_front.column+b_last.column)/2+1;
    b011.line=(b_front.line+b_last.line)/2;
    b011.column=b_last.column;
    //6
    b100.line=(b_front.line+b_last.line)/2+1;
    b100.column=b_front.column;
    b101.line=b_last.line;
    b101.column=(b_front.column+b_last.column)/2;
    //7
    b110.line=(b_front.line+b_last.line)/2+1;
    b110.column=(b_front.column+b_last.column)/2+1;
    b111.line=b_last.line;
    b111.column=b_last.column;
    //8

    div_matrix(a000,a001,b000,b001);
    div_matrix(a010,a011,b100,b101);
    div_matrix(a000,a001,b010,b011);
    div_matrix(a010,a011,b110,b111);
    div_matrix(a100,a101,b000,b001);
    div_matrix(a110,a111,b100,b101);
    div_matrix(a100,a101,b010,b011);
    div_matrix(a110,a111,b110,b111);
}