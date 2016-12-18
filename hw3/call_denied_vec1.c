/**
 * Tries to run system calls explicitely denied
 * to sys_vector1
 */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/unistd.h>
#include <sys/syscall.h>

#define  CLONE_SYSCALLS 1

int main()
{
  pid_t pid;
  int ret = 0;
  mode_t mask = umask(0);
  umask(mask);

  printf("*********************************************************\n");
  printf("This program attempts to demonstrate call deny behavior\n");
  printf("for sys_vector1. The mkdir() and rmdir() system calls are\n");
  printf("explicitely denied to this vector. User code will not be\n");
  printf("able to call these syscalls() at all.\n");
  printf("*********************************************************\n\n");

  pid = syscall(/*__NR_xclone*/329, 1, 0);
  if (pid == 0 ) {
    printf("\nCalling mkdir() to create directory test000_mkdir\n");
    ret = syscall(__NR_mkdir, "./test000_mkdir", mask);
    if (ret < 0) {
      perror("Failed to create test000_mkdir");
    } else {
      printf("Successfully executed mkdir!\n");
    }

    printf("\nCalling rmdir() to delete directory test000_rmdir\n");
    printf("Please note that this directory need not exist\n");
    printf("as the call is denied at the entry point itself.\n");
    ret = syscall(__NR_mkdir, "./test000_rmdir", mask);
    if (ret < 0) {
      perror("Failed to delete test000_rmdir");
    } else {
      printf("Successfully executed rmdir!\n");
    }
  }
  exit(0); 
}
