#include<stdio.h>
#include<stdlib.h>
#define N 300
typedef struct{
    int line,column;
}kk;
typedef struct{
    int **mymatrix;
    int size;
}matrix;
matrix C;
void add_matrix(matrix *mtr,matrix A,matrix B);
void sub_matrix(matrix *mtr,matrix A,matrix B);
void inital(matrix *mtr,int size);
void strassen(matrix *mtr,kk c_front,kk c_last,matrix A,matrix B);

int main()
{
    matrix A,B;
    inital(&A,N);
    inital(&B,N);
    inital(&C,N);
    kk c_front,c_last;
    c_front.line=c_front.column=0;
    c_last.line=c_last.column=N-1;
    int i;
    for(i=0;i<N;i++)
    {
        A.mymatrix[i][i]=2;
    }
    add_matrix(&B,A,B);
    strassen(&C,c_front,c_last,A,B);
    return 0;
}
void add_matrix(matrix *mtr,matrix A,matrix B)
{
    int i,j;
    for(i=0;i<A.size;i++)
    {
        for(j=0;j<A.size;j++)
        {
            mtr->mymatrix[i][j]=A.mymatrix[i][j]+B.mymatrix[i][j];
        }
    }
}
void sub_matrix(matrix *mtr,matrix A,matrix B)
{
    int i,j;
    for(i=0;i<A.size;i++)
    {
        for(j=0;j<A.size;j++)
        {
            mtr->mymatrix[i][j]=A.mymatrix[i][j]-B.mymatrix[i][j];
        }
    }
}
void inital(matrix *mtr,int size)
{
    int i;
    mtr->size=size;
    mtr->mymatrix=(int **)calloc(size,sizeof(int *));
    for(i=0;i<size;i++)
    {
        mtr->mymatrix[i]=(int *)calloc(size,sizeof(int ));
    }
}
void strassen(matrix *mtr,kk c_front,kk c_last,matrix A,matrix B)
{
    if(c_front.line==c_last.line&&c_front.column==c_last.column)
    {
        mtr->mymatrix[0][0]=A.mymatrix[0][0]+B.mymatrix[0][0];
        return;
    }
    matrix a00,a01,a10,a11,b00,b01,b10,b11;
    matrix d1,d2,d3,d4,d5,d6,d7;
    matrix temp,temp1,c00,c01,c10,c11;
    int mysize=A.size/2;
    inital(&a00,mysize);
    inital(&a01,mysize);
    inital(&a10,mysize);
    inital(&a11,mysize);
    inital(&b00,mysize);
    inital(&b01,mysize);
    inital(&b10,mysize);
    inital(&b11,mysize);
    inital(&d1,mysize);
    inital(&d2,mysize);
    inital(&d3,mysize);
    inital(&d4,mysize);
    inital(&d5,mysize);
    inital(&d6,mysize);
    inital(&d7,mysize);
    inital(&temp,mysize);
    inital(&temp1,mysize);
    inital(&c00,mysize);
    inital(&c01,mysize);
    inital(&c10,mysize);
    inital(&c11,mysize);

    int i,j;
    for(i=0;i<A.size;i++)
    {
        for(j=0;j<A.size;j++)
        {
            if(i<mysize&&j<mysize)
            {
                a00.mymatrix[i][j]=A.mymatrix[i][j];
                b00.mymatrix[i][j]=B.mymatrix[i][j];
            }
            else if(i<mysize&&j>=mysize)
            {
                a01.mymatrix[i][j-mysize]=A.mymatrix[i][j];
                b01.mymatrix[i][j-mysize]=B.mymatrix[i][j];
            }
            else if(i>=mysize&&j<mysize)
            {
                a10.mymatrix[i-mysize][j]=A.mymatrix[i][j];
                b10.mymatrix[i-mysize][j]=A.mymatrix[i][j];
            }
            else
            {
                a11.mymatrix[i-mysize][j-mysize]=A.mymatrix[i][j];
                b11.mymatrix[i-mysize][j-mysize]=B.mymatrix[i][j];
            }
        }
    }
    kk d_front,d_last;
    d_front.line=d_front.column=0;
    d_last.line=d_last.column=mysize-1;
    //计算d1——d7
    //1
    add_matrix(&temp,a00,a11);
    add_matrix(&temp1,b00,b11);
    strassen(&d1,d_front,d_last,temp,temp1);
    //2
    add_matrix(&temp,a10,a11);
    strassen(&d2,d_front,d_last,temp,b00);
    //3
    sub_matrix(&temp,b01,b11);
    strassen(&d3,d_front,d_last,a00,temp);
    //4
    sub_matrix(&temp,b10,b00);
    strassen(&d4,d_front,d_last,a11,temp);
    //5
    add_matrix(&temp,a00,a01);
    strassen(&d5,d_front,d_last,temp,b11);
    //6
    sub_matrix(&temp,a10,a00);
    add_matrix(&temp1,b00,b01);
    strassen(&d6,d_front,d_last,temp,temp1);
    //7
    sub_matrix(&temp,a01,a11);
    add_matrix(&temp1,b10,b11);
    strassen(&d7,d_front,d_last,temp,temp1);

    add_matrix(&temp,d1,d4);
    sub_matrix(&temp1,temp,d5);
    add_matrix(&c00,temp1,d7);

    add_matrix(&c01,d3,d5);

    add_matrix(&c10,d2,d4);

    add_matrix(&temp,d1,d3);
    sub_matrix(&temp1,temp,d2);
    add_matrix(&c11,temp1,d6);

    for(i=0;i<A.size;i++)
    {
        for(j=0;j<A.size;j++)
        {
            if(i<mysize&&j<mysize)
            {
                mtr->mymatrix[i][j]=c00.mymatrix[i][j];
            }
            else if(i<mysize&&j>=mysize)
            {
                mtr->mymatrix[i][j]=c01.mymatrix[i][j-mysize];
            }
            else if(i>=mysize&&j<mysize)
            {
                mtr->mymatrix[i][j]=c10.mymatrix[i-mysize][j];
            }
            else
            {
                mtr->mymatrix[i][j]=c11.mymatrix[i-mysize][j-mysize];
            }
        }
    }
}