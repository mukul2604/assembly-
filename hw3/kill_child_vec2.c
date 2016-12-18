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
  pid = syscall(329, 2);
  
  if (pid == 0) {
    ppid = getpid();
    printf("\npid with denied kill: %d\n", ppid);

    if(kill(ppid, SIGKILL) < 0)
      printf("Failed when kill itself pid=%d\n",ppid);
    
    pid = syscall(329,2);
    if (pid == 0) {
        /* do nothing */
        while(1); 
    } else {
      printf("parent_pid=%d child_pid=%d\n", ppid, pid);
      printf("sleeping for 10 seconds to check child process\n");
      sleep(10);
      printf("sleep finished\n");
      if (kill(pid, SIGKILL) < 0)
        printf("Failed when kill the child\n");
      else
        printf("Killed child\n");
      /* do nothing */
      while(1);
    }
  }
  exit(0);
}
