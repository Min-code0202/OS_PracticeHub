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

 //공유 메모리에 채팅과 상태 각각 프로세스의 id를 저장함 
struct memory {
	char buff[100];
	int status, pid1, pid2;
};

 //공유 메모리를 가리키는 포인터 
struct memory* shmptr;

//eixt를 입력받으면 종료함
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

	int shmid;
	
	//메모리 공유를 위한 키
	int key = 12345;
	
	//shmget로 공유 메모리 생성, 함수와 인덱스의 의미는 아래에 기술함
	shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
	
	//shmat로 공유메모리에 접근
	shmptr = (struct memory*)shmat(shmid, NULL, 0);
	
	//공유 메모리에 user1의 pid와 status를 저장함
	shmptr -> pid1 = pid;
	shmptr -> status = NotReady;

	//종료 시그널
	signal(SIGINT, exit_handler);
	//SIGUSR1 시그널이 오면 실행
	signal(SIGUSR1, handler);

	while(1){
		//status가 Ready이면 대기함
		if(shmptr -> status != Ready)
			printf("수신대기...\n");

		while(shmptr -> status != Ready)
			continue;
		sleep(1);
		
		//메시지를 입력하고 status를 FILLED로 변경
		printf("입력대기:\n ");
		fgets(shmptr->buff, 100, stdin);

		if(strcmp(shmptr->buff, "exit\n") == 0){
			//종료 시그널
			kill(shmptr -> pid2, SIGINT);
			exit(0);
		}
		shmptr -> status = FILLED;
		
		//user2 프로세스에게 SIGUSR2 시그널 보냄
		kill(shmptr -> pid2, SIGUSR2);
	}
	shmdt((void*)shmptr);
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
