#include "sys_xclone.h"
#include <asm/unistd.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/sched.h>
#include <asm/signal.h>

#define MODULE_VEC_ID 2
#define PPSV_MODNAME "childMurderer"

static struct sys_vector **sys_vec;
static int32_t vector_id = MODULE_VEC_ID;

int initialize_vector(sys_vector** s);
void release_vector(sys_vector** s);
long vec_2_mkdir(long, long, long, long, long, long);
long vec_2_kill(long, long, long, long, long, long);
long vec_2_unlink(long, long, long, long, long, long);

//========== Syscall definitions  ==========
long vec_2_mkdir(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  mode_t mode = (mode_t)si;
  struct task_struct *current_task = current;
  printk(KERN_DEBUG "Inside vector2 mkdir\n");
  printk(KERN_ALERT "Mode: %d", (int) mode);
  printk(KERN_ALERT "Current task name: %s", current_task->comm);
  if (strstr(current_task->comm, "httpd") != NULL) {
    printk(KERN_ALERT "httpd isn't allowed to create directories!");
    return -EFAULT;
  }
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_2_kill(long di,  long si, long dx,
                long r10, long r8, long r9)
{
    int pid, sig;
    struct task_struct *child;

    pid  = (int)di;
    sig = (int)si;

    if (pid < 0) {
        VDBG;
        return -EINVAL;
    }

    if (sig >= SIGUNUSED || sig < SIGHUP) {
        VDBG;
        return -EINVAL;
    }

    child = find_task_by_vpid(pid);
    printk("vec2kill:%d, current:%d\n",pid,current->pid);
    if (child == NULL) {
      VDBG;
      return -ESRCH;
    }

    if (child->real_parent->pid != current->pid ||
        child->parent->pid != current->pid) {
      printk(KERN_DEBUG "Inside vector2 kill::DENY\n");
      return PPSV_CALL_DENY_X86;
    } else {
      printk(KERN_DEBUG "Inside vector2 kill:: FOLLOW\n");
      return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
    }
}

long vec_2_unlink(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char __user* pathname = (char *)di;
  char *pathname_kernel = NULL;
  if (pathname != NULL) 
    pathname_kernel = (char *)kmalloc(strlen_user(pathname), GFP_KERNEL);
  if (pathname_kernel == NULL) {
    printk("Unable to allocate mem for kernel pathname buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(pathname_kernel, pathname, strlen_user(pathname)) < 0) {
    printk(KERN_ALERT "Copy of pathname to kernel failed!");
    kfree(pathname_kernel);
    return -EACCES;
  }
  printk(KERN_DEBUG "Inside vector2 Unlink\n");
  printk(KERN_ALERT "Path: %s", pathname_kernel);
  if ((strstr(pathname_kernel, ".PROTECTED") != NULL) || (strstr(pathname_kernel, ".protected") != NULL)) {
    printk(KERN_ALERT "Attempt made to delete .protected file, rejected!");
    kfree(pathname_kernel);
    return -EFAULT;
  }
  kfree(pathname_kernel);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

//========== Generic syscall vector module functionality  ==========
/**
 * Initializes a new sysvector
 */
int initialize_vector(sys_vector** s)
{
  short int i = 1;
  int ret = 0;
  *s = (sys_vector*)kmalloc(sizeof(sys_vector), GFP_KERNEL);
  if (*s == NULL) {
    printk("Failed to allocate mem for sys vector\n");
    ret = -ENOMEM;
    return ret;
  }

  // Initialize refcnt to 0
  (*s)->refcnt = 0;

  // Initialize refcnt lock
  mutex_init(&((*s)->refcnt_lock));

  // Clean up the entire systabl
  for (i = 0; i < PPSV_MAX_SYSCALLS; i++) {
    *(((*s)->systabl) + i) = PPSV_DEFAULT_CALL;
  }

  // Assign -1 to each element of the process list
  for (i = 0; i < PPSV_MAX_PROCESSES; i++) {
    *(((*s)->proc_list) + i) = -1;
  }

  (*s)->mod = THIS_MODULE;
  printk(KERN_ALERT "(*s)->mod->name: %s", (*s)->mod->name);
  printk(KERN_ALERT "(*s)->mod->addr: %p  ", (*s)->mod);

  // Initialize proc_list lock
  mutex_init(&((*s)->proc_list_lock));

  // Deny certain calls
  *(((*s)->systabl) + __NR_chdir) = PPSV_DENY_CALL;
  *(((*s)->systabl) + __NR_rmdir) = PPSV_DENY_CALL;

  // Make this vector point to our calls
  *(((*s)->systabl) + __NR_mkdir) = (unsigned long)vec_2_mkdir;
  *(((*s)->systabl) + __NR_kill) = (unsigned long)vec_2_kill;
  *(((*s)->systabl) + __NR_unlink) = (unsigned long)vec_2_unlink;

  return ret;
}

/**
 * Checks if this vector is releasable. If so, releases.
 */
void release_vector(sys_vector** s)
{
  // Release memory
  printk(KERN_ERR "Releasing sysvector memory\n");
  kfree(*s);
  // Check refcount. If 0, you can release.
  return;
}

/**
 * init_sys_vector - initialize the sys_vector module
 */
static int __init init_sys_vector(void)
{
  int ret = 0;

  init_super_list();
  // grab kobj lock
  printk("list initialized %d %p\n", super_list_initialized, &super_list_initialized);
  if (is_registered_vector(vector_id)) {
    printk(KERN_ERR "Module is already loaded\n");
    return -EPERM;
  }

  sys_vec = register_and_get_vector(vector_id);
  if (sys_vec == NULL) {
    printk(KERN_ERR "Failed to get sys vector for module %d", vector_id);
    return -ENOMEM;
  }

  ret = initialize_vector(sys_vec);
  if (ret != 0) {
    printk(KERN_ALERT "Module load failed!");
    return ret;
  }
  printk (KERN_DEBUG "%s module loaded\n", PPSV_MODNAME);
  
  /*TODO* : remove it from here once ioctl code is available*/
  dump_vector_list();
  return 0;
}

/**
 * exit_sys_vector - remove sys_vector module
 */ 
static void  __exit exit_sys_vector(void)
{
  int32_t refc = check_vec_refcount(*sys_vec);
  if (refc == 0) {
    // Release from super list
    (void)unregister_vector(vector_id);
    // Release the vector held by this module
    release_vector(sys_vec);
    printk(KERN_DEBUG "%s module unloaded\n", PPSV_MODNAME);
    dump_vector_list();
  }
}

module_init(init_sys_vector);
module_exit(exit_sys_vector);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CSE506P02");
