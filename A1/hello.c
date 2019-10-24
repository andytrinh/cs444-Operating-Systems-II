#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
//if f == 0: child process
//if f > 0: parent process
int main() {
  pid_t  pid;
  int f = fork();
  pid = getpid();
  if (f == 0) { // child process
    fprintf(stdout, "hello\n");
  }
  else if (f > 0) { // parent process
    sleep(1);
    fprintf(stdout, "goodbye\n");
  }
}
