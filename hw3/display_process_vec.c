#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>

#include "vec_ioctl.h"

/* X Macros for syscall table */

#define X(a,b,c) a=b,
enum SYSCALL {
#include "syscall_table.h"
};
#undef X

#define X(a,b,c) c,
char *syscall_name[] = {
#include "syscall_table.h"
};
#undef X

/* function for ioctl call */

void display_process_vec(int fd, pid_t pid)
{
	int ret = 0;
	struct display_process_vec_arg *display_process_vec_arg;
	int pid_cnt, syscall_denied_cnt, syscall_custom_cnt;
	int itr;
	enum SYSCALL syscall_id;

	display_process_vec_arg = malloc(sizeof(struct display_process_vec_arg));

	if (display_process_vec_arg == NULL)
	{
		fprintf(stderr, "cannot allocate memory for display_process_vec_arg\n");
		exit(-1);
	}

	display_process_vec_arg->pid = pid;

	ret = ioctl(fd, VEC_IOCTL_DISPLAY_PROCESS_VEC, display_process_vec_arg);
	
	if (ret != 0)
	{
		fprintf(stderr, "display_process_vec failed: %d\n", errno);
		exit(-1);
	}

	/* For the processes with default vector, just print the Id */
	if (display_process_vec_arg->vec_info.vec_id == 0) {
		printf("The given process has been assigned default system call vector.\n");
		printf("Vector Id: 0\n");
		exit(0);
	}

	/* print complete vec_info for custom vectors */

	printf("******************************\n");
	printf("Vector Id:%d\n", display_process_vec_arg->vec_info.vec_id);
	printf("Reference count: %d\n", display_process_vec_arg->vec_info.refcnt);
	pid_cnt = display_process_vec_arg->vec_info.pid_cnt;
	printf("\nSubscribed processes currently: %d\n", pid_cnt);
	printf("Subscribed processes details: \n");
	for (itr = 0; itr < pid_cnt; itr++)
	{
		printf("PID %d\n", display_process_vec_arg->vec_info.pid_list[itr]);
	}
	syscall_denied_cnt = display_process_vec_arg->vec_info.syscall_denied_cnt;
	syscall_custom_cnt = display_process_vec_arg->vec_info.syscall_custom_cnt;
	printf("\nVector Syscall information: \n");
	printf("Denied System calls: \n");
	for (itr = 0; itr < syscall_denied_cnt; itr++)
	{
		syscall_id = display_process_vec_arg->vec_info.syscall_denied_list[itr];
		printf("%s\n", syscall_name[syscall_id]);
	}
	printf("Available custom System calls: \n");
	for (itr = 0; itr < syscall_custom_cnt; itr++)
	{
		syscall_id = display_process_vec_arg->vec_info.syscall_custom_list[itr];
		printf("%s\n", syscall_name[syscall_id]);
	}
	printf("******************************\n");

}

/* main: call ioctl function */

int main(int argc, char * const argv[])
{
	char *file_name = DEVICE_FILE_PATH;
	int fd, i;
	pid_t pid;

	int opt;
	while ((opt = getopt(argc, argv, "p:")) != -1)
	{
		switch (opt)
		{
			case 'p':
				for (i=0; optarg[i]; i++) {
        			if (!isdigit(optarg[i])) {
            			printf("Input pid should be a number!\n");
            			exit(-1);
        			}
        		}
				pid = (pid_t) atoi(optarg);
				break;

			case '?':
				fprintf(stderr, "unknown option '-%C'\n", optopt);
				break;

			default:
				abort();
		}
	}

	fd = open(file_name, O_RDWR);
	if (fd < 0)
	{
		perror("can't open device file\n");
		return fd;
	}
	display_process_vec(fd, pid);

	return 0;
}
