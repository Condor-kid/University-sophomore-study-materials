#include<stdio.h>
int main()
{
    int A[300][300],B[300][300],C[300][300];
    int i,j,k;
    for(i=0;i<300;i++)
    {
        for(j=0;j<300;j++)
        {
            A[i][j]=B[i][j]=C[i][j]=2;
        }
    }
    //传统运算
    for(i=0;i<300;i++)
    {
        for(j=0;j<300;j++)
        {
            for(k=0;k<300;k++)
            {
                C[i][j]+=A[i][k]*B[k][j];
            }
        }
    }
    return 0;
}