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
  /* TODO: why this macro not working. */
  pid = syscall(/*__NR_xclone*/329, 1);
  if (pid == 0 ) {
    pid = syscall(/*__NR_xclone*/329, 2);
    pid = syscall(/*__NR_xclone*/329, CLONE_SYSCALLS);
    printf("child\n");
    sleep(5);
    perror("child");
  } else {
    pid = syscall(/*__NR_xclone*/329, 1);
    printf("parent\n");
    sleep(5);
    perror("parent");
  }
  printf("Now while(1)");
  while(1);
  exit(pid); 
}
