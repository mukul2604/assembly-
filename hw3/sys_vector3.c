#include "sys_xclone.h"
#include <asm/unistd.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/linkage.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/moduleloader.h>

#define MODULE_VEC_ID 3 // Use hash values here
#define PPSV_MODNAME "traceCalls"
#define TRACE_PATH "/tmp/vec3_trace.txt"
static struct sys_vector **sys_vec;
static int32_t vector_id = MODULE_VEC_ID;
struct file *file_ptr = NULL;

struct mutex file_lock;
int initialize_vector(sys_vector** s);
void release_vector(sys_vector** s);
long vec_3_write(long, long, long, long, long, long);
long vec_3_link(long, long, long, long, long, long);
long vec_3_symlink(long, long, long, long, long, long);
long vec_3_rmdir(long, long, long, long, long, long);
long vec_3_unlink(long, long, long, long, long, long);

//=========Helper functions ===============

void open_outfile(void)
{
  char *outfile = TRACE_PATH;
  mm_segment_t old_fs;
  file_ptr = filp_open(outfile, O_RDONLY, 0);
  if (!file_ptr || IS_ERR(file_ptr)) {}
  else {
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    vfs_unlink(file_ptr->f_path.dentry->d_parent->d_inode,
     file_ptr->f_path.dentry, NULL);
    set_fs(old_fs);
    filp_close(file_ptr, NULL);
  }
  file_ptr = filp_open(outfile, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  if (file_ptr == NULL || IS_ERR(file_ptr))
    printk(KERN_ALERT "Unable to open file in write mode!");
}

int write_file(void *buffer, int size)
{
  mm_segment_t old_fs;
  int bytes = 0;
  old_fs = get_fs();
  set_fs(KERNEL_DS);
  bytes = vfs_write(file_ptr, buffer, size, &file_ptr->f_pos);
  set_fs(old_fs);
  return bytes;
}

//========== Syscall definitions  ==========
long vec_3_write(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char *deco = "\n=========================================\n";
  char __user *buf = (char *) si;
  char *file_path = NULL;
  struct task_struct *current_task = current;
  int fd = (int) di;
  char *proc_name = current_task->comm;
  int fill_path_len = 256;
  char *fill_path = kzalloc(256, GFP_KERNEL);
  size_t count = (size_t) dx;
  struct file *target = current_task->files->fd_array[fd];

  char *buf_kernel = NULL;
  if (buf != NULL) 
    buf_kernel = (char *)kmalloc(strlen_user(buf), GFP_KERNEL);
  if (buf_kernel == NULL) {
    printk("Unable to allocate mem for kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(buf_kernel, buf, strlen_user(buf)) < 0) {
    printk(KERN_ALERT "Copy of buf to kernel failed!");
    kfree(buf_kernel);
    return -EACCES;
  }

  if (target == NULL) {
    printk(KERN_ALERT "Unable to retrieve file structure for fd");
    return -EFAULT;
  }
  printk(KERN_ALERT "Fd = %d", fd);
  file_path = dentry_path_raw(target->f_path.dentry, fill_path, fill_path_len);
  printk(KERN_ALERT "File_path: %s", file_path);
  printk(KERN_ALERT "Bytes to write: %d", (int) count);
  mutex_lock(&file_lock);
  write_file(deco, strlen(deco));
  write_file("\n Process: ", 12);
  write_file(proc_name, strlen(proc_name));
  write_file("\n Operation: WRITE", 19);
  write_file("\n File_write_path: ", 20);
  write_file(file_path, strlen(file_path));
  write_file("\n Content: ", 12);
  write_file(buf_kernel, strlen(buf_kernel));
  write_file(deco, strlen(deco));
  mutex_unlock(&file_lock);
  kfree(buf_kernel);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}


long vec_3_unlink(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char *deco = "\n=========================================\n";
  char __user *buf = (char *) di;

  char *buf_kernel = NULL;
  struct task_struct *current_task = current;
  char *proc_name = current_task->comm;
  if (buf != NULL) 
    buf_kernel = (char *)kmalloc(strlen_user(buf), GFP_KERNEL);
  if (buf_kernel == NULL) {
    printk("Unable to allocate mem for buf_kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(buf_kernel, buf, strlen_user(buf)) < 0) {
    printk(KERN_ALERT "Copy of buf to kernel failed!");
    kfree(buf_kernel);
    return -EACCES;
  }

  printk(KERN_ALERT "File_path: %s", buf_kernel);
  mutex_lock(&file_lock);
  write_file(deco, strlen(deco));
  write_file("\n Process: ", 12);
  write_file(proc_name, strlen(proc_name));
  write_file("\n Operation: UNLINK", 22);
  write_file("\n File_path: ", 13);
  write_file(buf_kernel, strlen(buf_kernel));
  write_file(deco, strlen(deco));
  mutex_unlock(&file_lock);
  kfree(buf_kernel);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_3_link(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char *deco = "\n=========================================\n";
  char __user *old_buf = (char *) di;
  char __user *new_buf = (char *) si;

  char *old_kernel = NULL;
  char *new_kernel = NULL;
  struct task_struct *current_task = current;
  char *proc_name = current_task->comm;
  if (old_buf != NULL) 
    old_kernel = (char *)kmalloc(strlen_user(old_buf), GFP_KERNEL);
  if (old_kernel == NULL) {
    printk("Unable to allocate mem for old_kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(old_kernel, old_buf, strlen_user(old_buf)) < 0) {
    printk(KERN_ALERT "Copy of old_buf to kernel failed!");
    kfree(old_kernel);
    return -EACCES;
  }

  if (new_buf != NULL) 
    new_kernel = (char *)kmalloc(strlen_user(new_buf), GFP_KERNEL);
  if (new_kernel == NULL) {
    printk("Unable to allocate mem for new_kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(new_kernel, new_buf, strlen_user(new_buf)) < 0) {
    printk(KERN_ALERT "Copy of new_buf to kernel failed!");
    kfree(new_kernel);
    return -EACCES;
  }

  printk(KERN_ALERT "File_path: %s", old_kernel);
  printk(KERN_ALERT "Link_path: %s", new_kernel);
  mutex_lock(&file_lock);
  write_file(deco, strlen(deco));
  write_file("\n Process: ", 12);
  write_file(proc_name, strlen(proc_name));
  write_file("\n Operation: HARDLINK", 22);
  write_file("\n File_path: ", 13);
  write_file(old_kernel, strlen(old_kernel));
  write_file("\n Link_path: ", 13);
  write_file(new_kernel, strlen(new_kernel));
  write_file(deco, strlen(deco));
  mutex_unlock(&file_lock);
  kfree(old_kernel);
  kfree(new_kernel);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_3_rmdir(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char *deco = "\n=========================================\n";
  char __user *buf = (char *) di;

  char *buf_kernel = NULL;
  struct task_struct *current_task = current;
  char *proc_name = current_task->comm;
  if (buf != NULL) 
    buf_kernel = (char *)kmalloc(strlen_user(buf), GFP_KERNEL);
  if (buf_kernel == NULL) {
    printk("Unable to allocate mem for buf_kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(buf_kernel, buf, strlen_user(buf)) < 0) {
    printk(KERN_ALERT "Copy of buf to kernel failed!");
    kfree(buf_kernel);
    return -EACCES;
  }

  printk(KERN_ALERT "Directory_path: %s", buf_kernel);
  mutex_lock(&file_lock);
  write_file(deco, strlen(deco));
  write_file("\n Process: ", 12);
  write_file(proc_name, strlen(proc_name));
  write_file("\n Operation: RMDIR", 18);
  write_file("\n Directory_path: ", 18);
  write_file(buf_kernel, strlen(buf_kernel));
  write_file(deco, strlen(deco));
  mutex_unlock(&file_lock);
  kfree(buf_kernel);
  return PPSV_SUGGEST_DEFAULT_FOLLOWUP;
}

long vec_3_symlink(long di,  long si, long dx,
                long r10, long r8, long r9)
{
  char *deco = "\n=========================================\n";
  char __user *old_buf = (char *) di;
  char __user *new_buf = (char *) si;

  char *old_kernel = NULL;
  char *new_kernel = NULL;
  struct task_struct *current_task = current;
  char *proc_name = current_task->comm;
  if (old_buf != NULL) 
    old_kernel = (char *)kmalloc(strlen_user(old_buf), GFP_KERNEL);
  if (old_kernel == NULL) {
    printk("Unable to allocate mem for old_kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(old_kernel, old_buf, strlen_user(old_buf)) < 0) {
    printk(KERN_ALERT "Copy of old_buf to kernel failed!");
    kfree(old_kernel);
    return -EACCES;
  }

  if (new_buf != NULL) 
    new_kernel = (char *)kmalloc(strlen_user(new_buf), GFP_KERNEL);
  if (new_kernel == NULL) {
    printk("Unable to allocate mem for new_kernel buf");
    return -ENOMEM;
  }
  if (strncpy_from_user(new_kernel, new_buf, strlen_user(new_buf)) < 0) {
    printk(KERN_ALERT "Copy of new_buf to kernel failed!");
    kfree(new_kernel);
    return -EACCES;
  }

  printk(KERN_ALERT "File_path: %s", old_kernel);
  printk(KERN_ALERT "Link_path: %s", new_kernel);
  mutex_lock(&file_lock);
  write_file(deco, strlen(deco));
  write_file("\n Process: ", 12);
  write_file(proc_name, strlen(proc_name));
  write_file("\n Operation: SYMLINK", 21);
  write_file("\n File_path: ", 13);
  write_file(old_kernel, strlen(old_kernel));
  write_file("\n Link_path: ", 13);
  write_file(new_kernel, strlen(new_kernel));
  write_file(deco, strlen(deco));
  mutex_unlock(&file_lock);
  kfree(old_kernel);
  kfree(new_kernel);
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

  // Open trace file
  open_outfile();

  // Initialize refcnt lock
  mutex_init(&((*s)->refcnt_lock));

  // Initialize file lock
  mutex_init(&file_lock);

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
  *(((*s)->systabl) + __NR_read) = PPSV_DENY_CALL;

  // Make this vector point to our calls
  *(((*s)->systabl) + __NR_link) = (unsigned long)vec_3_link;
  *(((*s)->systabl) + __NR_symlink) = (unsigned long)vec_3_symlink;
  *(((*s)->systabl) + __NR_write) = (unsigned long)vec_3_write;
  *(((*s)->systabl) + __NR_rmdir) = (unsigned long)vec_3_rmdir;
   *(((*s)->systabl) + __NR_unlink) = (unsigned long)vec_3_unlink;

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
