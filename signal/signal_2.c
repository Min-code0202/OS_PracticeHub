#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>


int main(){
	//공유 메모리로 signal_1 프로그램의 PID를 받아옴
	int key = 1234;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	int* shmptr = (int*)shmat(shmid, NULL, 0);
	//signal_1로 시그널 보냄
	kill(*shmptr, SIGUSR1);

	return 0;
}
