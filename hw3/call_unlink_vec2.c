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
  pid = syscall(/*__NR_xclone*/329, 2);
  if (pid == 0) {
    printf("child\n");
    sleep(5);
    printf("Now execute unlink: file1.txt\n");
    ret = syscall(SYS_unlink, "file1.txt");
    if (ret != 0) {
      printf("unlink operation: file1.txt failed!: permission denied\n");
    }
    else {
      printf("unlink operation was successfull !\n");
    }
  } else {
    pid = syscall(/*__NR_xclone*/329, 2);
    if (pid == 0) {
      printf("child\n");
      sleep(5);
      printf("Now execute unlink: file2.protected\n");
      ret = syscall(SYS_unlink, "file2.protected");
      if (ret != 0) {
        printf("unlink operation: file2.protected failed!: permission denied\n");
      }
      else {
        printf("unlink operation was successfull !\n");
      }
    }
  }
  exit(pid); 
}
