#include<stdio.h>
#include<unistd.h>

int main(int argc, char* argv[]){
	pid_t pid = fork();

	int value = 0;

	printf("Hello World1 value = %d\n", value++);
	printf("Hello World2 value = %d\n", value++);

	return 0;
}
