#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "vec_ioctl.h"

/* function for ioctl call */

void update_process_vec(int fd, int pid, int vec_id)
{
	int ret = 0;
	struct update_process_vec_arg *update_process_vec_arg;

	update_process_vec_arg = malloc(sizeof(struct update_process_vec_arg));

	if (update_process_vec_arg == NULL)
	{
		fprintf(stderr, "cannot allocate memory for update_process_vec_arg\n");
		exit(-1);
	}

	update_process_vec_arg->pid = pid;
	update_process_vec_arg->vec_id = vec_id;

	ret = ioctl(fd, VEC_IOCTL_UPDATE_PROCESS_VEC, update_process_vec_arg);

	if (ret != 0)
	{
		fprintf(stderr, "update_process_vec failed: %d\n ", errno);
		exit(-1);
	}
}

/* main: call ioctl function */

int main(int argc, char * const argv[])
{
	char *file_name = DEVICE_FILE_PATH;
	int fd;
	int pid, vec_id;

	int opt;
	while ((opt = getopt(argc, argv, "p:v:")) != -1)
	{
		switch (opt)
		{
			case 'p':
				/* IMPORTANT: since atoi is used to perform string to int
				* conversion, if the user inputs a non-number, it is considered
				* to be 0 */
				
				pid = atoi(optarg);
				break;

			case 'v':
				/* IMPORTANT: same warning as before */
			
				vec_id = atoi(optarg);
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

	update_process_vec(fd, pid, vec_id);
	
	return 0;
}
