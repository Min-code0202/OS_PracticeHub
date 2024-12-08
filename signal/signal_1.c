#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>

//CTRL+C를 입력받으면 종료할지 물음
void sigint_handler(int sig){
        printf("\n종료할까요? (y/n): ");
        char c = getchar();
        if(c == 'y')
                exit(0);
        else{
                printf("계속 실행합니다.\n");
                getchar();
        }
}

void sigusr1_handler(int sig){
        printf("터미널 2에서 SIGUSR1 시그널 발생\n");
}

int main(){
	//현재 프로세스 ID
	int pid = getpid();
	printf("ID: %d\n", pid);
	//signal_2와 공유할 메모리 생성
	int key = 1234;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	int* shmptr = (int*)shmat(shmid, NULL, 0);

	*shmptr = pid;

        signal(SIGINT, sigint_handler);
        signal(SIGUSR1, sigusr1_handler);

        int count = 0;
        printf("프로그램을 실행합니다. CTRL+C 입력 시 종료.\n");
        while(1){
                printf("%d\n", count++);
                sleep(1);
        }

        return 0;
}
