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
#include <signal.h>
#include <sys/wait.h>

#define  CLONE_SYSCALLS -1

int main(int argc, char *argv[])
{
	pid_t ppid, pid=0;
  //int status;
  pid = syscall(/*__NR_xclone*/329, 2);
  
  if (pid == 0) {
    ppid = getpid();
    printf("\npid with denied kill: %d\n", ppid);

    if(kill(ppid, SIGKILL) < 0)
      printf("Failed when kill itself\n");
    
    pid = syscall(329, 2);
    if (pid == 0) {
      if (kill(ppid, SIGKILL) < 0)
        printf("Failed when kill the parent\n");
    } else {
      printf("parent process in while loop\n");
      while(1);
    }
  }
  exit(0);
}
