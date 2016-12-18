obj-m += sys_vector1.o
obj-m += sys_vector2.o
obj-m += sys_vector3.o
obj-m += vec_ioctl.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: modules user

user:
	gcc -Wall -Werror -C xclone.c -o bin/xclone
	gcc -Wall -Werror -C kill_parent_vec2.c -o bin/kill_parent_vec2
	gcc -Wall -Werror -C kill_child_vec2.c -o bin/kill_child_vec2
	gcc -Wall -Werror -C display_vectors.c -o bin/display_vectors
	gcc -Wall -Werror -C update_process_vec.c -o bin/update_process_vec
	gcc -Wall -Werror -C display_process_vec.c -o bin/display_process_vec
	gcc -Wall -Werror -C program_launcher.c -o bin/program_launcher
	gcc -Wall -Werror -C call_mkdir_vec2.c -o bin/call_mkdir_vec2
	gcc -Wall -Werror -C httpd_mkdir_vec2.c -o bin/httpd_mkdir_vec2
	gcc -Wall -Werror -C call_unlink_vec2.c -o bin/call_unlink_vec2
	gcc -Wall -Werror -C trace_write_vec3.c -o bin/trace_write_vec3
	gcc -Wall -Werror -C trace_hardlink_vec3.c -o bin/trace_hardlink_vec3
	gcc -Wall -Werror -C trace_symlink_vec3.c -o bin/trace_symlink_vec3
	gcc -Wall -Werror -C trace_rmdir_vec3.c -o bin/trace_rmdir_vec3
	gcc -Wall -Werror -C trace_unlink_vec3.c -o bin/trace_unlink_vec3
	gcc -Wall -Werror -C vec3_deny_test.c -o bin/vec3_deny_test
	gcc -Wall -Werror -C vec2_deny_test.c -o bin/vec2_deny_test
	gcc -Wall -Werror -C call_chdir_vec1.c -o bin/call_chdir_vec1
	gcc -Wall -Werror -C call_denied_vec1.c -o bin/call_denied_vec1
	gcc -Wall -Werror -C call_open_creat_vec1.c -o bin/call_open_creat_vec1

modules:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f bin/xclone
	rm -f bin/kill_parent_vec2
	rm -f bin/kill_child_vec2
	rm -f bin/display_vectors
	rm -f bin/update_process_vec
	rm -f bin/display_process_vec
	rm -f bin/program_launcher
	rm -f bin/call_mkdir_vec2
	rm -f bin/httpd_mkdir_vec2
	rm -f bin/call_unlink_vec2
	rm -f bin/trace_write_vec3
	rm -f bin/trace_hardlink_vec3
	rm -f bin/trace_unlink_vec3
	rm -f bin/trace_symlink_vec3
	rm -f bin/trace_rmdir_vec3
	rm -f bin/vec3_deny_test
	rm -f bin/vec2_deny_test
	rm -f bin/call_chdir_vec1
	rm -f bin/call_denied_vec1
	rm -f bin/call_open_creat_vec1
