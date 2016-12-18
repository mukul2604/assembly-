#include "sys_xclone.h"
#include <linux/file.h>
#include <asm/unistd.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <linux/fdtable.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>

#define MODULE_VEC_ID 1
#define PPSV_MODNAME "pickyWriter"

static struct sys_vector **sys_vec;
static int32_t vector_id = MODULE_VEC_ID;

void release_vector(sys_vector** s);
int initialize_vector(sys_vector** s);
long vec_1_chdir (long, long, long, long, long, long);
long vec_1_fchdir(long, long, long, long, long, long);
long vec_1_open  (long, long, long, long, long, long);
long vec_1_creat (long, long, long, long, long, long);

//========== Syscall definitions  ==========
long vec_1_open(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char __user* filename = (char*)di;
  int flags = (int)si;
  char *process_name = current->comm;
  int process_name_len = strlen(process_name);

  printk(KERN_ERR "Inside vector1 open\n");
  if (!(strncmp(filename, process_name, process_name_len) == 0
       && filename[process_name_len] == '_'))
  {
    if (flags & O_CREAT) {
      printk(KERN_ALERT "File name %s does not begin with %s_\n",
                         filename, process_name);
      return -EPERM;
    }
  }
  printk(KERN_ALERT "Opening file %s\n", filename);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_1_creat(long di,  long si, long dx,
                 long r10, long r8, long r9)
{
  char __user* filename = (char*)di;
  char *process_name = current->comm;
  int process_name_len = strlen(process_name);

  printk(KERN_ERR "Inside vector1 creat\n");
  if (!(strncmp(filename, process_name, process_name_len) == 0
                && filename[process_name_len] == '_'))
  {
    printk(KERN_ALERT "File name %s does not begin with %s_\n",
    filename, process_name);
    return -EPERM;
  }
  printk(KERN_ALERT "Creating file %s\n", filename);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_1_chdir(long di,  long si, long dx,
                 long r10, long r8, long r9)
{
  char __user * udirname = (char*)di;
  char* pathname_kernel = NULL;
  printk(KERN_ERR "Inside vector1 chdir\n");
  if (udirname != NULL) {
    pathname_kernel = (char *)kmalloc(strlen_user(udirname),
                                      GFP_KERNEL);
  }
  if (pathname_kernel == NULL) {
    printk("Unable to allocate mem for kernel pathname buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(pathname_kernel,
                        udirname,
                        strlen_user(udirname)) < 0) {
    printk(KERN_ALERT "Copy of pathname to kernel failed!");
    kfree(pathname_kernel);
    return -EACCES;
  }

  if (strstr(pathname_kernel, "/etc") != NULL) {
    printk(KERN_ALERT "Attempt made to chdir to /etc. Denying access!");
    kfree(pathname_kernel);
    return -EPERM;
  }
  kfree(pathname_kernel);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_1_fchdir(long di,  long si, long dx,
                  long r10, long r8, long r9)
{
  int fd = (int)di;
  char* file_path = NULL;
  int fill_path_len = 256;
  char *fill_path = kzalloc(256, GFP_KERNEL);
  struct task_struct *current_task = current;
  struct file *target = current_task->files->fd_array[fd];

  printk(KERN_ERR "Inside vector1 fchdir\n");
  if (target == NULL) {
    printk(KERN_ALERT "Unable to retrieve file structure for fd");
    return -EFAULT;
  }
  printk(KERN_ALERT "Fd = %d", fd);
  file_path = dentry_path_raw(target->f_path.dentry, fill_path, fill_path_len);
  if (IS_ERR(file_path) || file_path == NULL) {
    printk(KERN_ALERT "PPSV: fchdir: file_path not set\n");
    return -EIO;
  }
  printk(KERN_ALERT "File_path: %s", file_path);

  if (strstr(file_path, "/etc") != NULL) {
    printk(KERN_ALERT "Attempt made to fchdir to /etc. Denying access!");
    kfree(fill_path);
    return -EPERM;
  }
  kfree(fill_path);
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
  *(((*s)->systabl) + __NR_mkdir) = PPSV_DENY_CALL;
  *(((*s)->systabl) + __NR_rmdir) = PPSV_DENY_CALL;

  // Make this vector point to our calls
  *(((*s)->systabl) + __NR_open) = (unsigned long)vec_1_open;
  *(((*s)->systabl) + __NR_chdir) = (unsigned long)vec_1_chdir;
  *(((*s)->systabl) + __NR_fchdir) = (unsigned long)vec_1_fchdir;
  *(((*s)->systabl) + __NR_creat) = (unsigned long)vec_1_creat;

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
