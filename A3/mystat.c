#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <stdlib.h>
#include <grp.h>

int findType(struct stat st);
void convert(struct stat st);
char printMode(mode_t mode);
void substring(char s[], char sub[], int p, int l);


int findType(struct stat st) {
  //figure out what type if file it is (directory, pipes, symbolic links, sockets)
  if (S_ISREG(st.st_mode)) {
    return 0;
  }
  else if (S_ISDIR(st.st_mode)) {
    return 1;
  }
  else if (S_ISCHR(st.st_mode)) {
    return 2;
  }
  else if (S_ISBLK(st.st_mode)) {
    return 3;
  }
  else if (S_ISFIFO(st.st_mode)) {
    return 4;
  }
  else if (S_ISLNK(st.st_mode)) {
    return 5;
  }
  else if (S_ISSOCK(st.st_mode)) {
    return 6;
  }
  else {
    return -1;
  }
}

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

char printMode(mode_t mode) {
  printf((mode & S_IRUSR) ? "r" : "-");
  printf((mode & S_IWUSR) ? "w" : "-");
  printf((mode & S_IXUSR) ? "x" : "-");
  printf((mode & S_IRGRP) ? "r" : "-");
  printf((mode & S_IWGRP) ? "w" : "-");
  printf((mode & S_IXGRP) ? "x" : "-");
  printf((mode & S_IROTH) ? "r" : "-");
  printf((mode & S_IWOTH) ? "w" : "-");
  printf((mode & S_IXOTH) ? "x" : "-");
  return 0;
}

int main(int argc, char **argv) {
  struct passwd *pwd;
  struct group *grp;
  struct stat st;
  struct tm *t1 = NULL;
  struct tm *t2 = NULL;
  struct tm *t3 = NULL;
  char *buf = NULL;
  char str[10];
  char sub[30];
  ssize_t nbytes = 0, bufsize = 0;
  char buffer1[80];
  char buffer2[80];
  char buffer3[80];
  int index = 0, i = 0;
  char type[7][25] = {"regular file", "directory", "character device",
                      "block device", "FIFO/pipe", "Symbolic Link", "socket"};

  if (argc < 2) {
    fprintf(stderr, "Invalid number of arguments\n");
    return -1;
  }

  for (i = 1; i < argc; i++) {
    if(lstat(argv[i], &st) == -1) {
      perror("stat");
      return -1;
    }

    bufsize = st.st_size + 2;
    buf = malloc(bufsize);
    if(buf == NULL) {
      fprintf(stderr, "malloc error\n");
      return -1;
    }
    nbytes = readlink(argv[i], buf, bufsize); //buf contains file that symlink points to

    index = findType(st); //get file type

    pwd = getpwuid(st.st_uid); //pwd->pw_name
    grp = getgrgid(st.st_gid); // grp->gr_name

    sprintf(str, "%lo", (unsigned long)st.st_mode); //get last three digits of octal value
    substring(str, sub, strlen(str)-2, strlen(str));

    t1 = localtime(&st.st_atime);
    t2 = localtime(&st.st_mtime);
    t3 = localtime(&st.st_ctime);
    strftime(buffer1, 80,"%Y-%m-%d %H:%M:%S %z (%Z) %a (local)", t1);
    strftime(buffer2, 80,"%Y-%m-%d %H:%M:%S %z (%Z) %a (local)", t2);
    strftime(buffer3, 80,"%Y-%m-%d %H:%M:%S %z (%Z) %a (local)", t3);

    printf("File: %s\n", argv[i]);
    if(nbytes == -1) {
      printf(" File type:                  %s\n", type[index]);
    }
    else if (strcmp(buf, "JUNK") == 0) {
      printf(" File type:                  %s -> with dangling destination\n", type[index]);
    }
    else {
      printf(" File type:                  %s -> %s\n", type[index], buf);
    }
    printf(" Device ID number:           %d\n", (int)st.st_dev);
    printf(" I-node number:              %ld\n", (long)st.st_ino);
    printf(" Mode:                       ");

    if (S_ISREG(st.st_mode)) {
      printf("-");
    }
    else if (S_ISDIR(st.st_mode)) {
      printf("d");
    }
    else if (S_ISCHR(st.st_mode)) {
      printf("c");
    }
    else if (S_ISBLK(st.st_mode)) {
      printf("b");
    }
    else if (S_ISFIFO(st.st_mode)) {
      printf("p");
    }
    else if (S_ISLNK(st.st_mode)) {
      printf("l");
    }
    else if (S_ISSOCK(st.st_mode)) {
      printf("s");
    }
    else {
      printf("?");
    }
    printMode(st.st_mode);
    printf("        (%s in octal)\n", sub);
    printf(" Link count:                 %ld\n", (long)st.st_nlink);
    printf(" Owner Id:                   %s\t       (UID = %ld)\n", pwd->pw_name, (long)st.st_uid);
    printf(" Group Id:                   %-8.8s\t       (GID = %ld)\n", grp->gr_name, (long)st.st_gid);
    printf(" Preferred I/O block size:   %ld bytes\n", (long)st.st_blksize);
    printf(" File size:                  %lld bytes\n", (long long)st.st_size);
    printf(" Blocks allocated:           %lld\n", (long long)st.st_blocks);
    printf(" Last file access:           %s\n", buffer1);
    printf(" Last file modification:     %s\n", buffer2);
    printf(" Last status change:         %s\n", buffer3);
  }

  return 0;
}
