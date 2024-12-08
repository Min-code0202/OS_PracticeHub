#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<unistd.h>

int THREAD_COUNT;
long TARGET_NUMBER; 

long mutex_sum = 0;

//pthread_mutex_t lock;

void* sum_range(void* arg){
	long start = *((long*)arg);
	long end = start + (TARGET_NUMBER / THREAD_COUNT) - 1;
	long sum = 0;
	
	for(int i = start; i <= end; i++)
		sum += (long)i;
	//critical section에 진입하기 전 잠금
//	pthread_mutex_lock(&lock);
	mutex_sum += sum;
	//잠금해제
//	pthread_mutex_unlock(&lock);

	return NULL;
}

void do_mutex_thread(){
	pthread_t threads[THREAD_COUNT];

	long thread_args[THREAD_COUNT];
	//mutex 초기화
//	pthread_mutex_init(&lock, NULL);

	for(int i = 0; i < THREAD_COUNT; i++){
		thread_args[i] = (i * (TARGET_NUMBER / THREAD_COUNT)) + 1;
		pthread_create(&threads[i], NULL, sum_range, &thread_args[i]);
	}
	for(int i = 0; i < THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);
	//mutex 파기
//	pthread_mutex_destroy(&lock);
}

int main(int argc, char* argv[]){
	if(argc != 3){
		fprintf(stderr, "Error\n");
		exit(1);
	}

	THREAD_COUNT = atoi(argv[1]);
	TARGET_NUMBER = atol(argv[2]);

	do_mutex_thread();
	printf("%ld\n", mutex_sum);

	return 0;
}
