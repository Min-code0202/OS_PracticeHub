#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/time.h>
#include<unistd.h>

#define READ_THREAD 10

//mutex 기반 RW_Lock
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;

int read_count = 0;
//critical section
//0으로 초기화
int resource = 0;

void* write_lock(void *arg){
        int id = *((int*) arg);//현재 write thread ID
        pthread_mutex_lock(&rw_mutex);
        if(id == 0){
                for(int i = 0; i < 2; i++){
                        resource = i + 1;
                        printf("write_ID[%d] resource: %d\n", id, resource);
                }
        }
        else{
                for(int i = 2; i < 4; i++){
                        resource = i + 1;
                        printf("write_ID[%d] resource: %d\n", id, resource);
                }
        }
        pthread_mutex_unlock(&rw_mutex);

        return NULL;
}

void* read_lock(void *arg){
        int id = *((int*) arg);//현재 read thread ID
        pthread_mutex_lock(&mutex);
        read_count++;
        if(read_count == 1)
                pthread_mutex_lock(&rw_mutex);
        pthread_mutex_unlock(&mutex);

        printf("read_ID[%d] resource: %d\n", id, resource);

        pthread_mutex_lock(&mutex);
        read_count--;
        if(read_count == 0)
                pthread_mutex_unlock(&rw_mutex);
        pthread_mutex_unlock(&mutex);

        return NULL;
}
//mutex기반 시간 측정
struct timeval rw_run(){
        struct timeval start, end;

        pthread_t reader[READ_THREAD];
        pthread_t writer[2];

        int read_id[READ_THREAD];
        int write_id[2];
        //시간 측정 시작
        gettimeofday(&start, NULL);
        for(int i = 0; i < 2; i++){
                write_id[i] = i;
                pthread_create(&writer[i], NULL, write_lock, &write_id[i]);
        }
        for(int i = 0; i < READ_THREAD; i++){
                read_id[i] = i;
                pthread_create(&reader[i], NULL, read_lock, &read_id[i]);
        }
        for(int i = 0; i < 2; i++)
                pthread_join(writer[i], NULL);
        for(int i = 0;  i < READ_THREAD; i++)
                pthread_join(reader[i], NULL);
        //시간 측정 종료
        gettimeofday(&end, NULL);
        timersub(&end, &start, &end);

        return end;
}

//cond 기반 rwlock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int cond_count = 0;
int cond_resource = 0;

void* cond_write_lock(void *arg){
	int id = *((int*) arg);
	pthread_mutex_lock(&rw_lock);
	if(id == 0){
		for(int i = 0; i < 2; i++){
			cond_resource = i + 1;
			printf("write_ID[%d] resource: %d\n", id, cond_resource);
		}
	}
	else{
		for(int i = 2; i < 4; i++){
			cond_resource = i + 1;
			printf("write_ID[%d] resource: %d\n", id, cond_resource);
		}
	}
	pthread_mutex_unlock(&rw_mutex);
	pthread_cond_broadcast(&cond);
	return NULL;
}

void* cond_read_lock(void *arg){
	int id = *((int*) arg);
	pthread_mutex_lock(&mutex);
	
	while(cond_resource == 0)
		pthread_cond_wait(&cond, &lock);
	cond_count++;

	if(cond_count == 1)
		pthread_mutex_lock(&rw_lock);
	pthread_mutex_unlock(&lock);
	cond_count--;

	if(cond_count == 0)
		pthread_mutex_unlock(&rw_lock);
	pthread_mutex_unlock(&lock);
	return NULL;
}

struct timeval cond_run(){
	struct timeval start, end;

	pthread_t read[READ_THREAD];
	pthread_t write[2];

	int read_ID[READ_THREAD];
	int write_ID[2];

	gettimeofday(&start, NULL);
	for(int i = 0; i < 2; i++){
		write_ID[i] = i;
		pthread_create(&write[i], NULL, cond_write_lock, &write_ID[i]);
	}
	for(int i = 0; i < READ_THREAD; i++){
		read_ID[i] = i;
		pthread_create(&read[i], NULL, cond_read_lock, &read_ID[i]);
	}
	for(int i = 0; i < 2; i++)
		pthread_join(write[i], NULL);
	for(int i = 0; i < READ_THREAD; i++)
		pthread_join(read[i], NULL);
	gettimeofday(&end, NULL);
	timersub(&end, &start, &end);

	return end;
}

void print_diff(struct timeval rw_time, struct timeval cond_time){
        double diffTime = 0;

        double rw = (double)(rw_time.tv_sec * 100000 + rw_time.tv_usec) / 100000;
        double cond = (double)(cond_time.tv_sec * 100000 + cond_time.tv_usec) / 100000;

        diffTime = rw / cond;
        printf("\nmutex 기반 RW Lock 시간: %lf | cond 기반  RW Lock 시간: %lf\n", rw, cond);
        printf("cond 기반  RW Lock를 사용한 시간이 mutex보다 %lf배 빨 라짐\n", diffTime);
}

int main(){
        printf("mutex 기반 RW Lock\n");
        struct timeval rw_time = rw_run();
        printf("===========================\n");
        printf("cond 기반 RW Lock\n");
        struct timeval cond_time = cond_run();

        print_diff(rw_time, cond_time);

        return 0;
}
