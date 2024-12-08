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

int main(){
        pthread_t reader[READ_THREAD];
        pthread_t writer[2];
	//thread ID
        int read_id[READ_THREAD];
        int write_id[2];
        
        for(int i = 0; i < 2; i++){
                write_id[i] = i;
                pthread_create(&writer[i], NULL, write_thread, &write_id[i]);
        }
	
	for(int i = 0; i < READ_THREAD; i++){
                read_id[i] = i;
                pthread_create(&reader[i], NULL, read_thread, &read_id[i]);
        }
        
	for(int i = 0;  i < READ_THREAD; i++)
                pthread_join(reader[i], NULL);
        for(int i = 0; i < 2; i++)
                pthread_join(writer[i], NULL);

	return 0;
}
