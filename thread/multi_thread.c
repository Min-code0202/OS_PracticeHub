#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<unistd.h>


//입력받을 thread 수와 1부터 더할 범위를 저장할 변수
int THREAD_COUNT;
long TARGET_NUMBER;

//single thread, multi thread에서 합을 저장할 변수
//multi_sum은 critical section으로 지정함
long single_sum = 0;
long multi_sum = 0;

//multi_sum에 thread의 동시 접근을 막기 위해 선언
pthread_mutex_t lock;

//single thread로 계산
struct timeval do_single_thread(){
	//시간 측정을 위한 변수
	struct timeval start, end, time;
	//single_sum을 구하기 전 시간
	gettimeofday(&start, NULL);
	for(int i = 1; i < TARGET_NUMBER + 1; i++)
		single_sum += (long)i;
	//single_sum을 구한 뒤 시간
	gettimeofday(&end, NULL);
	//timeval operations 중 하나로
	//single_sum을 연산하는데 걸린 시간 time = end - start
	timersub(&end, &start, &time);
	return time;
}

//thread의 start_routine
void* sum_range(void* arg){
        //int pointer에 arg를 casting하여 start 변수에 저장
        long start = *((long*)arg);
        long end = start + (TARGET_NUMBER / THREAD_COUNT) - 1;
        long sum = 0;
        for(int i = start; i <= end; i++)
                sum += (long)i;
        //critical section에 진입하기전 mutex를 잠금
        pthread_mutex_lock(&lock);
        //thread가 주어진 범위만큼 계산한 결과 저장
        multi_sum += sum;
        //잠금해제
        pthread_mutex_unlock(&lock);

        return NULL;
}

//multi thread로 계산
struct timeval do_multi_thread(){
        struct timeval start, end, time;

        //thread의 ID
        pthread_t threads[THREAD_COUNT];

        //각 thread 별로 처리할 범위 지정
        long thread_args[THREAD_COUNT];

        /* mutex 초기화
         * 정적으로 초기화 pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER
         * 동적으로 초기화 pthread_mutex_init()
         */
        pthread_mutex_init(&lock, NULL);

        //multi_sum을 구하기 전 시간
        gettimeofday(&start, NULL);

        //thread_args[]에 시작범위 저장
        //thread ID에 따라 thread를 생성하여 sum_range(start_routine)를 시작
        for(int i = 0; i < THREAD_COUNT; i++){
                thread_args[i] = (i * (TARGET_NUMBER / THREAD_COUNT)) + 1;
                pthread_create(&threads[i], NULL, sum_range, &thread_args[i]);
        }
        //thread를 종료함
        for(int i = 0; i < THREAD_COUNT; i++)
                pthread_join(threads[i], NULL);

        //multi_sum을 구한 뒤 시간
        gettimeofday(&end, NULL);
        //multi_sum을 구하는데 걸린 시간 time = end - start
        timersub(&end, &start, &time);
        //mutex를 동적으로 생성할 경우 mutex 객체를 파기함
        pthread_mutex_destroy(&lock);

        return time;
}

void print_diff(struct timeval single, struct timeval multi){
        double diffTime = 0;//소요시간 차이

        double single_time = (double)(single.tv_sec * 100000 + single.tv_usec) / 100000;
        double multi_time = (double)(multi.tv_sec * 100000 + multi.tv_usec) / 100000;
        //수행시간 차이 계산
        diffTime = single_time / multi_time;

        printf("single_thread:\n");
        printf("-결과: %ld, 소요시간: %lf초\n\n", single_sum, single_time);
        printf("multi_thread:\n");
        printf("-결과: %.ld, 소요시간: %lf초\n\n", multi_sum, multi_time);
        printf("시간 비교:\n");
        printf("multi thread가 single thread 대비 %lf배 빠르게 수행되었음.\n", diffTime);
}

int main(int argc, char* argv[]){
	if(argc != 3){
		fprintf(stderr, "Error! 사용할 쓰레드의 수와 1부터 더할 숫자를 입력하시오( 예시> ./(name) 4 100 )\n");
		exit(1);
	}
	//입력받은 사용할 thread 수와 숫자를 long형으로 변환
	THREAD_COUNT = atoi(argv[1]);
	TARGET_NUMBER = atol(argv[2]);
        struct timeval single_thread_processing_time = do_single_thread();
        struct timeval multi_thread_processing_time = do_multi_thread();

        print_diff(single_thread_processing_time, multi_thread_processing_time);

        return 0;
}
