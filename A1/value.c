#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int xx = 100;
  pid_t  pid;
  int f = fork();
  pid = getpid();
  if (f == 0) { // child process
    printf("Old Child Process value: %d\n", xx);
    xx = 777;
    printf("New Child Process value: %d\n", xx);
  }
  else if (f > 0) { // parent process
    printf("Old Parent Process: %d\n", xx);
    xx = 999;
    printf("New Parent Process: %d\n", xx);
  }
}
