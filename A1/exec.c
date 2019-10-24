#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  pid_t pid;
  char * ls_args[] = { "ls" , "-l", "-F", "-h", (char*)0};

  //int execl(const char *path, const char *arg,.../* (char  *) NULL */);\
  //takes a string as the path for the first arg
  if ((pid = fork()) == -1)
    perror("fork error");
  else if (pid == 0) {
    execl("/bin/ls", "ls", "-l", "-F", "-h", (char*)0);
  }
  else {
    wait(NULL);
  }

  //execlp() = (const char *file, const char *arg,.../* (char  *) NULL */);
  //takes a file pointer as the first arg
  if ((pid = fork()) == -1)
    perror("fork error");
  else if (pid == 0) { // child process
    execlp("ls", "ls", "-l", "-F", "-h",(char*)0);
  }
  else {
    wait(NULL);
  }
  //int execv(const char *path, char *const argv[]);
  //takes the string path then an array for the rest of the args
  if ((pid = fork()) == -1)
    perror("fork error");
  else if (pid == 0) { // child process
    execv("/bin/ls", ls_args);
  }
  else {
    wait(NULL);
  }
  //int execvp (const char *file, char *const argv[]);
  //takes a file pointer as the first arg then an array for the rest
  if ((pid = fork()) == -1)
    perror("fork error");
  else if (pid == 0) { // child process
    execvp("ls", ls_args);
  }
  else {
    wait(NULL);
  }

  if (fork() > 0) { // parent process
    wait(NULL);
  }
  return 0;
}

//These four system calls replaces the current running process with a
//new one. In this program, they all produce the same output because of
//the provided arguments despite the fact that they take a different
//format of arguments.
