#include<stdio.h>
#include<time.h>
#include<sys/time.h>
#include<stdlib.h>
#include<pthread.h>

//线程数 
#define thread_number 8
#define vec_length 10000000

//两个向量 
float arr1[vec_length];
float arr2[vec_length];
//结果 
float result[vec_length];
//线程调用的函数 
void *vec_sum(void *t);

int main()
{
    srand(time(NULL));
    long thread;
	int i;
    pthread_t *ss;
    ss=(pthread_t*)malloc(thread_number*sizeof(pthread_t));
    for(i=0;i<vec_length;i++)
    {
        arr1[i]=rand()%999;
        arr2[i]=rand()%999;
    }
    struct timeval t1,t2;
    gettimeofday(&t1,NULL);
    //创建线程
    for(thread=0;thread<thread_number;thread++)
    {
        pthread_create(&ss[thread],NULL,vec_sum,(void *)thread);
    }
    //结束线程
    for(thread=0;thread<thread_number;thread++)
    {
        pthread_join(ss[thread],NULL);
    }
    gettimeofday(&t2,NULL);
    long time1,time2;
    //多线程计算时间
    time1=(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec;
    //for循环单线程计算
    gettimeofday(&t1,NULL);
    for(i=0;i<vec_length;i++)
    {
        result[i]=arr1[i]+arr2[i];
    }
    gettimeofday(&t2,NULL);
    //for循环单线程计算时间 
    time2=(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec;
    printf("phread_time: %ld μs\n",time1);
    printf("for_loop_time: %ld μs\n",time2);
    return 0;
}
void *vec_sum(void *t)
{
    long tt=(long)t;
    int number=vec_length/thread_number;
    int first=tt*number;
    int last=(tt+1)*number;
    int i;
    for(i=first;i<last;i++)
    {
        result[i]=arr1[i]+arr2[i];
    }
    return NULL;
}