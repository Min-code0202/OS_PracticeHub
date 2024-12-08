#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
  
#define FILLED 0
#define Ready 1
#define NotReady -1
  
struct memory {
	char buff[100];
	int status, pid1, pid2;
};

struct memory* shmptr;

//exit 입력 받으면 종료
void exit_handler(int signum){
	shmptr -> status = FILLED;
	exit(0);
}
//user1에게 메시지가 오면 출력
void handler(int signum){
	//SIGUSR2 시그널이 오면 user1의 메시지를 출력함
	if(signum == SIGUSR2){
		printf("A) ");
		puts(shmptr -> buff);
	}
}

int main(){
	//현재 프로세스(user2)의 pid
	int pid = getpid();

	int shmid;

	int key = 12345;

	shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
	
	
	shmptr = (struct memory*)shmat(shmid, NULL, 0);
	
	//공유 메모리에 user2의 pid와 status를 저장함
	shmptr -> pid2 = pid;
	shmptr -> status = NotReady;
	
	//종료 시그널
	signal(SIGINT, exit_handler);
	//SIGUSR2 시그널이 오면 실행
	signal(SIGUSR2, handler);

	while(1){
		sleep(1);

		printf("입력대기:\n");
		fgets(shmptr->buff, 100, stdin);

		if(strcmp(shmptr->buff, "exit\n") == 0){
			//종료 시그널 전송
			kill(shmptr->pid1, SIGINT);
			exit(0);
		}
		shmptr -> status = Ready;

		//user1 프로세스에게 SIGUSR1 시그널 보냄	
		kill(shmptr -> pid1, SIGUSR1);
		
		if(shmptr -> status == Ready)
			printf("수신대기...\n");
		
		while(shmptr -> status == Ready)
			continue;
	}
	shmdt((void*)shmptr);
	return 0;
}
