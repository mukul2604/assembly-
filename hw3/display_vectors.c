#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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

/* functions for ioctl calls */

void print_vec_info(int fd)
{
	int ret = 0;
	struct display_vec_info *display_vec_info;
	int vec_count;
	int syscall_denied_cnt;
	int syscall_custom_cnt;
	int pid_cnt;
	int itr1, itr2;
	enum SYSCALL syscall_id;

	display_vec_info = malloc(sizeof(struct display_vec_info));
	
	if (display_vec_info == NULL)
	{
		printf("cannot allocate memory for display_vec_info\n");
		exit(-1);
	}
	
	ret = ioctl(fd, VEC_IOCTL_DISPLAY_VEC_INFO, display_vec_info);

	if (ret != 0)
	{
		printf("display_vec_info failed: %d\n", ret);
		exit(-1);
	}

	/* print vec_info */
	vec_count = display_vec_info->vec_count;
	printf("Number of vectors registered: %d\n\n", vec_count);

	for (itr1 = 0; itr1 < vec_count; itr1++)
	{
		printf("******************************\n");
		printf("Vector# %d\n", itr1 + 1);
		printf("Vector Id:%d\n", display_vec_info->vec_info_arr[itr1].vec_id);
		printf("Reference count: %d\n", display_vec_info->vec_info_arr[itr1].refcnt);
		pid_cnt = display_vec_info->vec_info_arr[itr1].pid_cnt;
		printf("\nSubscribed processes currently: %d\n", pid_cnt);
		printf("Subscribed processes details: \n");
		for (itr2 = 0; itr2 < pid_cnt; itr2++)
		{
			printf("PID %d\n", display_vec_info->vec_info_arr[itr1].pid_list[itr2]);
		}
		syscall_denied_cnt = display_vec_info->vec_info_arr[itr1].syscall_denied_cnt;
		syscall_custom_cnt =  display_vec_info->vec_info_arr[itr1].syscall_custom_cnt;
		printf("\nVector Syscall information: \n");
		printf("Denied System calls: \n");
		for (itr2 = 0; itr2 < syscall_denied_cnt; itr2++)
		{
			syscall_id = display_vec_info->vec_info_arr[itr1].syscall_denied_list[itr2];
			printf("%s\n", syscall_name[syscall_id]);
		}
		printf("Available custom System calls: \n");
		for (itr2 = 0; itr2 < syscall_custom_cnt; itr2++)
		{
			syscall_id = display_vec_info->vec_info_arr[itr1].syscall_custom_list[itr2];
			printf("%s\n", syscall_name[syscall_id]);
		}
		printf("******************************\n");
	}
}

/* main: call ioctl functions */

int main()
{
	char *file_name = DEVICE_FILE_PATH;
	int fd;

	fd = open(file_name, O_RDWR);
	if (fd < 0)
	{
		perror("can't open device file\n");
		return fd;
	}
	print_vec_info(fd);

	return 0;
}
