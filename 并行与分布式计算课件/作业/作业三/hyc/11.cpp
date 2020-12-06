#include <pthread.h>
#include<stdio.h>
int a;
void *Thread1(void *x) {
  int y=*((int*)x);
  a = 32+y;
  return x;
}
int main() {
  pthread_t t;
  int x=35; 
  pthread_create(&t, NULL, Thread1, (void*)&x);
  a = 43-x;
  pthread_join(t, NULL);
  printf("%d",a);
  return 0;
}
