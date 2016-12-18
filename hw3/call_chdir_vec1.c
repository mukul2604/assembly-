/**
 * Tests chdir() and fchdir() system calls from sys_vector1
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
  int fd = 0;
  int ret = 0;

  printf("*********************************************************\n");
  printf("This program attempts to demonstrate change directory\n");
  printf("restrictions. The custom system calls in this vector \n");
  printf("do not allow users to chdir or fchdir to /etc directory.\n");
  printf("*********************************************************\n\n");

  pid = syscall(/*__NR_xclone*/329, 1, 0);
  if (pid == 0 ) {
    printf("Calling chdir() to go to /etc directory\n");
    ret = syscall(__NR_chdir, "/etc");
    if (ret < 0) {
      perror("Failed to chdir to /etc");
    }
    printf("Calling chdir() to go to /home directory\n");
    ret = syscall(__NR_chdir, "/home");
    if (ret < 0) {
      perror("Failed to chdir to /etc");
    } else {
      printf("Successfully executed chdir!\n");
    }

    printf("Calling fchdir() to go to /etc directory\n");
    fd = open("/etc", O_RDONLY, 0);
    if (fd < 0) {
      printf("Failed to open /etc and receive FD for fchdir\n");
    }
    ret = syscall(__NR_fchdir, fd);
    if (ret < 0) {
        perror("Failed to fchdir() to /etc");
    }
    close(fd);
 
    printf("Calling fchdir() to go to /home directory\n");
    fd = open("/home", O_RDONLY, 0);
    if (fd < 0) {
      printf("Failed to open /home and receive FD for fchdir\n");
    }
    ret = syscall(__NR_fchdir, fd);
    if (ret < 0) {
      perror("Failed to fchdir() to /home");
    } else {
      printf("Successfully executed fchdir!\n");
    }
    close(fd);
  }
  exit(0); 
}
