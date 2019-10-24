#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int i, j;
  pid_t  pid;
  FILE *junk = fopen("JUNK.txt", "w");
  if (junk == NULL) {
      puts("Failed to open file");
      return 0;
  }
  else {
    fputs("before fork\n", junk);
    int f = fork();
    pid = getpid();
    if (f == 0) { // child process
      for(j = 0; j < 10; j++) {
        fputs("child\n", junk);
      }
    }
    else if (f > 0) { //parent process
      for(i = 0; i < 10; i++) {
        fputs("parent\n", junk);
      }
    }
  }
  fclose(junk);
}
//What happens when they are writing to the file concurrently, i.e., at the same times
//When they are writing to the file concurrently, both can write to the same file but the
//order in which they print isn't consistent nor can it be predicted without using other
//functions. "Before fork" is being printed twice because the child process inherits what
//is in the buffer before the fork actually happens.
