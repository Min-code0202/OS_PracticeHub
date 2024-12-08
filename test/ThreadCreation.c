#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

int a = 0;

void* func(void* arg){
	printf("Hello world in Thread a = %d\n", a++);
}

int main(int argc, char* argv[]){
	pthread_t t1, t2, t3;

	pthread_create(&t1, NULL, func, NULL);
	pthread_create(&t2, NULL, func, NULL);
	pthread_create(&t3, NULL, func, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);

	return 0;
}
