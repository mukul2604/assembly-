#ifndef VEC_IOCTL_H
#define VEC_IOCTL_H

#include <linux/ioctl.h>

#define VEC_MAGIC_NUM 0xffffdead

/* ioctl command numbers */
#define VEC_IOCTL_DISPLAY_VEC_INFO_NUM 1
#define VEC_IOCTL_DISPLAY_PROCESS_VEC_NUM 2
#define VEC_IOCTL_UPDATE_PROCESS_VEC_NUM 3

/* ioctl commands */
#define VEC_IOCTL_DISPLAY_VEC_INFO _IOR(VEC_MAGIC_NUM, \
        VEC_IOCTL_DISPLAY_VEC_INFO_NUM, struct display_vec_info *)

#define VEC_IOCTL_DISPLAY_PROCESS_VEC _IOWR(VEC_MAGIC_NUM, \
        VEC_IOCTL_DISPLAY_PROCESS_VEC_NUM, struct display_process_vec_arg*)

#define VEC_IOCTL_UPDATE_PROCESS_VEC _IOW(VEC_MAGIC_NUM, \
        VEC_IOCTL_UPDATE_PROCESS_VEC_NUM, struct update_process_vec_arg *)

/* device file name */
#define DEVICE_FILE_NAME "vec_dev"
#define DEVICE_FILE_PATH "/dev/vec_dev"

/* ioctl display arg struct */

#define MAX_VEC_COUNT 10
#define MAX_SYSCALLS 100
#define MAX_PROCESSES 100

struct vec_info {
	int vec_id;
	int32_t refcnt;
	int syscall_denied_cnt;
	int syscall_custom_cnt;
	int pid_cnt;
	int syscall_denied_list[MAX_SYSCALLS];
	int syscall_custom_list[MAX_SYSCALLS];
	pid_t pid_list[MAX_PROCESSES];
} vec_info;

struct display_vec_info {
	int vec_count;
	struct vec_info vec_info_arr[MAX_VEC_COUNT];
} display_vec_info;

/* ioctl display process vector arg struct */
struct display_process_vec_arg {
	pid_t pid;
	struct vec_info vec_info;
} display_process_vec_arg;

/* ioctl update process vector arg struct */

struct update_process_vec_arg {
	int pid;
	int vec_id;
} update_process_vec_arg;

#endif



