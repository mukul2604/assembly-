#ifndef _SYS_XCLONE_H_
#define _SYS_XCLONE_H_

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/module.h>

#define VDBG printk(KERN_DEBUG "VDBG:%s:%s:%d\n", __FILE__, __func__, __LINE__)
#define CLONE_SYSCALLS -1
#define PPSV_MAX_SYSCALLS 100
#define PPSV_MAX_PROCESSES 100
#define PPSV_DEFAULT_CALL 0
#define PPSV_DENY_CALL 1
#define PPSV_SUGGEST_DEFAULT_FOLLOWUP -841841
#define PPSV_CALL_DENY_X86 -31337

/**
 * sys_vector and SYSTABL layout
 * This will be used by all modules to store
 * their own implementations of syscall vectors.
 */
struct sys_vector {
  int32_t refcnt;
  unsigned long systabl[PPSV_MAX_SYSCALLS];
  pid_t proc_list[PPSV_MAX_PROCESSES];
  struct mutex refcnt_lock;
  struct mutex proc_list_lock;
  struct module *mod;
};

typedef struct sys_vector sys_vector;

struct list_vector_id {
  int32_t vec_id;
  struct sys_vector *vec;
  struct list_head list;
};

extern bool super_list_initialized;
extern struct list_vector_id super_list;

extern void init_super_list(void);
extern struct sys_vector **register_and_get_vector(int32_t vec_id);
extern int unregister_vector(int32_t vec_id);
extern bool is_registered_vector(int32_t vec_id);
extern struct sys_vector *get_sys_vector(int32_t vector_id);
extern void get_vec_refcount(struct sys_vector *vec);
extern void put_vec_refcount(struct sys_vector *vec);
extern int32_t check_vec_refcount(struct sys_vector *vec);
extern int vec_attach_pid(struct sys_vector *vec, pid_t pid);
extern int vec_detach_pid(struct sys_vector *vec, pid_t pid);
extern pid_t *vec_curr_proc_list(struct sys_vector *vec);


extern void dump_vector_list(void);
#endif
