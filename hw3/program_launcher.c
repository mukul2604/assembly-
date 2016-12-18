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
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

int main(int argc, char * const argv[])
{
	pid_t pid;
	
	char *executable;
	int vec_id;
	char *executable_cpy;
	char *executable_filename;

	int opt;
	while ((opt = getopt(argc, argv, "e:v:")) != -1)
	{
		switch (opt)
		{
			case 'e':
				executable = optarg;
				break;

			case 'v':
				vec_id = atoi(optarg);
				break;

			case '?':
				fprintf(stderr, "unknown option '-%C'\n", optopt);
				break;

			default:
				abort();
		}
	}

	// TODO: macro 
	pid = syscall(329, vec_id);

	if (pid == 0)
	{
		printf("child process calling execl()\n");
		executable_cpy = (char *) malloc(strlen(executable));
		strcpy(executable_cpy, executable);
		executable_filename = (char *) malloc(strlen(executable));
		executable_filename = basename(executable_cpy);
		printf("filename: %s\n", executable_filename);
		printf("executable: %s\n", executable);
		if (execl(executable, executable_filename, NULL) == -1)
		{
			fprintf(stderr, "execl failed: %d", errno);
		}
		
		//free(executable_cpy);
		//free(executable_filename);
	}
	else
	{
		exit(-1);
	}

	return 0;
}
