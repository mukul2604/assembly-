/**
 * Tests open() and creat() system calls from sys_vector1
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

  printf("*********************************************************\n");
  printf("This program attempts to demonstrate file creation\n");
  printf("restrictions. The custom system calls in this vector\n");
  printf("allow creation of files only with <prog name>_  prefix\n");
  printf("*********************************************************\n\n");

  pid = syscall(/*__NR_xclone*/329, 1, 0);
  if (pid == 0 ) {
    printf("Calling open() to create file abc\n");
    fd = open("abc", O_RDONLY|O_CREAT,
              S_IRUSR|S_IWUSR|S_IWUSR|S_IROTH);
    if (fd < 0) {
      perror("Failed to create file");
    }
    printf("Calling creat() to create file abc\n");
    fd = creat("abc", S_IRUSR|S_IWUSR|S_IWUSR|S_IROTH);
    if (fd < 0) {
      perror("Failed to create file");
    }

    printf("Calling open() to create file call_open_creat_vec1_abc\n");
    fd = open("call_open_creat_vec1_abc", O_RDONLY|O_CREAT,
              S_IRUSR|S_IWUSR|S_IWUSR|S_IROTH);
    if (fd < 0) {
      perror("Failed to create file");
    } else {
      printf("File created successfully!\n");
    }
    printf("Calling creat() to create file call_open_creat_vec1_def\n");
    fd = creat("call_open_creat_vec1_def", S_IRUSR|S_IWUSR|S_IWUSR|S_IROTH);
    if (fd < 0) {
      perror("Failed to create file");
    } else {
      printf("File created successfully!\n");
    }
  }
  return 0; 
}
