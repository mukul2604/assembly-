#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>

#define  CLONE_SYSCALLS -1
#ifndef __NR_xclone
//error xclone system call not defined
#endif

 
void usage(void) {
  return;
}

int main(int argc, char *argv[])
{
  pid_t pid;
  char *buf = (char *)malloc(10*sizeof(char));
  int fd = 0;
  int ret = 0;
  pid = syscall(/*__NR_xclone*/329, 3);
  if (pid == 0) {
    printf("child\n");
    sleep(5);
    printf("Try executing mkdir\n");
    ret = syscall(SYS_mkdir, "malicious_dir");
    if (ret < 0) {
      printf("mkdir operation failed!\n");
    }
    else {
      printf("mkdir was successfull !\n");
    }
    printf("Try executing read\n");
    fd = open("file1.txt", O_RDONLY, 0644);
    printf("File descriptor opened successfully: %d\n", fd);
    printf("Now try reading file..\n");
    ret = syscall(SYS_read, fd, buf, 10);
    if (ret < 0) {
      printf("File read operation failed!\n");
    }
    else {
      printf("File read was successfull !\n");
    }
  } else {
    printf("parent\n");
    sleep(5);
  }
  if(buf != NULL)
    free(buf);
  exit(pid); 
}
