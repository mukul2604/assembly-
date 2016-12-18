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
  int ret = 0;
  /* TODO: why this macro not working. */
  pid = syscall(/*__NR_xclone*/329, 3);
  if (pid == 0) {
    printf("child\n");
    sleep(5);
    printf("Now execute hard link\n");
    ret = syscall(SYS_link, "file1.txt", "file1.link");
    if (ret < 0) {
      printf("Hard Link operation failed!\n");
    }
    else {
      printf("Hard Link operation was successfull !\n");
    }
  } else {
    printf("parent\n");
    sleep(5);
  }
  exit(pid); 
}
