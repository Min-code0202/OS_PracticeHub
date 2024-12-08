#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/time.h>
#include<unistd.h>


#define READ_THREAD 10

//mutex 기반 RW_Lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;

int count = 0;
//critical section
//0으로 초기화
int resource = 0;

void* write_thread(void *arg){
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

void* read_thread(void *arg){
        int id = *((int*) arg);//현재 read thread ID
        pthread_mutex_lock(&lock);
        count++;
        if(count == 1)
                pthread_mutex_lock(&rw_mutex);
        pthread_mutex_unlock(&lock);

        printf("read_ID[%d] resource: %d\n", id, resource);

        pthread_mutex_lock(&lock);
        count--;
        if(count == 0)
                pthread_mutex_unlock(&rw_mutex);
        pthread_mutex_unlock(&lock);

        return NULL;
}
//mutex기반 시간 측정
struct timeval mutex_run(){
        struct timeval start, end;

        pthread_t reader[READ_THREAD];
        pthread_t writer[2];

        int read_id[READ_THREAD];
        int write_id[2];
        //시간 측정 시작
        gettimeofday(&start, NULL);
        for(int i = 0; i < READ_THREAD; i++){
                read_id[i] = i;
                pthread_create(&reader[i], NULL, read_thread, &read_id[i]);
        }
        for(int i = 0; i < 2; i++){
                write_id[i] = i;
                pthread_create(&writer[i], NULL, write_thread, &write_id[i]);
        }
        for(int i = 0;  i < READ_THREAD; i++)
                pthread_join(reader[i], NULL);
        for(int i = 0; i < 2; i++)
                pthread_join(writer[i], NULL);
        //시간 측정 종료
        gettimeofday(&end, NULL);
        timersub(&end, &start, &end);

        return end;
}

//rwlock 사용
//정적으로 초기화
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

//rwlock으로 읽을 변수
//0으로 초기화
int rw_resource = 0;
int read_count = 0;

void write_lock(){
        pthread_mutex_lock(&write_mutex);
}

void write_unlock(){
        pthread_mutex_unlock(&write_mutex);
}

void read_lock(){
        pthread_mutex_lock(&mutex);
        read_count++;

        if(read_count == 1)
                pthread_mutex_lock(&write_mutex);
        pthread_mutex_unlock(&mutex);
}

void read_unlock(){
        pthread_mutex_lock(&mutex);
        read_count--;

        if(read_count == 0)
                pthread_mutex_unlock(&write_mutex);
        pthread_mutex_unlock(&mutex);
}

void* read_func(void *arg){
        //현재 thread ID
        int id = *((int*) arg);

        read_lock();
        printf("read_ID[%d] rw_resource: %d\n", id, rw_resource);
        read_unlock();

        return NULL;
}

void* write_func(void *arg){
        //현재 thread ID
        int id = *((int*) arg);
        if(id == 0){
                for(int i = 0; i < 2; i++){
                        write_lock();
                        rw_resource = i + 1;
                        printf("write_ID[%d] rw_resource: %d\n", id, rw_resource);
                        write_unlock();
                }
        }
        else{
                for(int i = 2; i < 4; i++){
                        write_lock();
                        rw_resource = i + 1;
                        printf("write_ID[%d] rw_resource: %d\n", id, rw_resource);
                        write_unlock();
                }
        }

        return NULL;
}

struct timeval rw_run(){
	struct timeval start, end;

        pthread_t reader[READ_THREAD];
        pthread_t writer[2];
        //출력할 때 해당 thread의 ID
        int read_id[READ_THREAD];
        int write_id[2];
	
	gettimeofday(&start, NULL);
        //read thread
        for(int i = 0; i < 10; i++){
                read_id[i] = i;
                pthread_create(&reader[i], NULL, read_func, &read_id[i]);
        }

        //write thread
        for(int i = 0; i < 2; i++){
                write_id[i] = i;
                pthread_create(&writer[i], NULL, write_func, &write_id[i]);
        }
        for(int i = 0; i < 10; i++)
                pthread_join(reader[i], NULL);
        for(int i = 0; i < 2; i++)
                pthread_join(writer[i], NULL);	
	gettimeofday(&end, NULL);
	timersub(&end, &start, &end);

	return end;
}

void print_diff(struct timeval rw_time, struct timeval mutex_time){
	double diffTime = 0;

	double rw = (double)(rw_time.tv_sec * 100000 + rw_time.tv_usec) / 100000;	
	double mutex = (double)(mutex_time.tv_sec * 100000 + mutex_time.tv_usec) / 100000;

	diffTime = mutex / rw;
	printf("RW Lock 시간: %lf | mutex 시간: %lf\n", rw, mutex);	
	printf("RW Lock를 사용한 시간이 mutex보다 %lf배 빨라짐\n", diffTime);
}

int main(){
	printf("RW Lock 사용\n");	
	struct timeval rw_time = rw_run();
	printf("\n===========================\n");	
	printf("mutex 사용\n");	
	struct timeval mutex_time = mutex_run();
	
	print_diff(rw_time, mutex_time);

        return 0;
}
