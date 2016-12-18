#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>

#define  CLONE_SYSCALLS 1
#ifndef __NR_xclone
//error xclone system call not defined
#endif

int main()
{
  pid_t pid;
  int err = 0;
  pid = syscall(/*__NR_xclone*/329, 2);

  if (pid == 0 ) {
    printf("child process with vector 2 and pid = %d\n",getpid());

    err = syscall(__NR_mkdir, "test", 755);
    if (err < 0) {
      perror("Error mkdir");
    } else {
      perror("create \"test\" directory");
    }

    err = syscall(__NR_chdir, "test");
    if (err < 0) {
        perror("Change dir to \"test\" is denied");
    }

    err = syscall(__NR_rmdir, "test");
    if (err < 0) {
        perror("Remove directory \"test\" is denied");
    }
  }
  exit(pid); 
}
