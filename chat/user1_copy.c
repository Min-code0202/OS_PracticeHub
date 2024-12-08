#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

//공유 메모리의 상태를 의미함
#define FILLED 0
#define Ready 1
#define NotReady -1

//공유 메모리에 채팅과 상태 각 프로세스의 id를 저장함 
struct memory {
	char buff[100];
	int status, pid1, pid2;
};

//공유 메모리를 가리키는 포인터
struct memory* shmptr;

//eixt 입력 받으면 종료
void exit_handler(int signum){
	shmptr -> status = FILLED;
	exit(0);
}

//user2에게 메시지가 오면 출력함
void handler(int signum)
{
 	//SIGUSR1으로 시그널이 오면 user2의 메시지를 출력함 
	if (signum == SIGUSR1) {
		printf("B) ");
		puts(shmptr->buff);
    }	
}

int main(){
	//현재 프로세스(user1)의 pid
	int pid = getpid();
	//공유 메모리  id
	int shmid;
	
	//메모리 공유를 위한 키
	int key = 12345;
	
	//shmget로 공유 메모리 생성, shm계열 함수의 자세한 내용은  아래에 기술함
	shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
	
	//shmat로 공유메모리에 접근
	shmptr = (struct memory*)shmat(shmid, NULL, 0);
	
	//공유 메모리에 user1의 pid와 status를 저장함
	shmptr -> pid1 = pid;
	shmptr -> status = NotReady;

	//종료 시그널
	signal(SIGINT, exit_handler);
	//SIGUSR1 시그널
	signal(SIGUSR1, handler);

	while(1){
		//메시지를 입력받음
		fgets(shmptr->buff, 100, stdin);
		//exit를 입력받으면 user2에게 종료시그널 전송 후 종료 
		if(strcmp(shmptr->buff, "exit\n") == 0){
			//종료 시그널
			kill(shmptr -> pid2, SIGINT);
			exit(0);
		}
		//메시지를 다 입력 받으면 status를 FILLED 전환
		shmptr -> status = FILLED;
		
		//user2 프로세스에게 SIGUSR2 시그널 보냄
		kill(shmptr -> pid2, SIGUSR2);
	}
	//연결된 공유메모리 분리
	shmdt((void*)shmptr);
	//공유 메모리 제거
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
