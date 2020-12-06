//AVX和for循环对比
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<immintrin.h>
#include<sys/time.h>

#define vec_length 1000000

//两个向量
float arr1[vec_length];
float arr2[vec_length];
//结果
float result[vec_length];
//两种计算方法
long avx_sum(float *arr1,float *arr2,float *result);
long for_loop_sum(float *arr1,float *arr2,float *result);
//包含8个float类型数字的向量
__m256 num1[vec_length/8];
__m256 num2[vec_length/8];
__m256 result2[vec_length/8];
float answer[vec_length];
long avx_sum(float *arr1,float *arr2,float *result)
{
    //先将向量转成avx的形式
    int i;
    for(i=0;i<vec_length;i+=8)
    {
        num1[i/8]=_mm256_set_ps(arr1[i],arr1[i+1],arr1[i+2],arr1[i+3],arr1[i+4],arr1[i+5],arr1[i+6],arr1[i+7]);
        num2[i/8]=_mm256_set_ps(arr2[i],arr2[i+1],arr2[i+2],arr2[i+3],arr2[i+4],arr2[i+5],arr2[i+6],arr2[i+7]);
    }
    struct timeval t1,t2;
    gettimeofday(&t1,NULL);
    for(i=0;i<vec_length/8;i++)
    {
        result2[i]=_mm256_add_ps(num1[i],num2[i]);
    }
    gettimeofday(&t2,NULL);
    //AVX计算时间
    return ((t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec);
}
long for_loop_sum(float *arr1,float *arr2,float *result)
{
    struct timeval t1,t2;
    int i;
    gettimeofday(&t1,NULL);
    for(i=0;i<vec_length;i++)
    {
        result[i]=arr1[i]+arr2[i];
    }
    gettimeofday(&t2,NULL);
    //for循环计算时间
    return ((t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec);
}
int main()
{
    srand(time(NULL));
    //生成随机向量
    int i;
    for(i=0;i<vec_length;i++)
    {
        arr1[i]=rand()%999;
        arr2[i]=rand()%999;
    }
    long time1,time2;
    time1=avx_sum(arr1,arr2,result);
    time2=for_loop_sum(arr1,arr2,result);
    printf("avx_time: %ld μs\n",time1);
    printf("for_loop_time: %ld μs\n",time2);
    return 0;
}
