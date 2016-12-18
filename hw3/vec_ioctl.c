#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include "vec_ioctl.h"
#include "sys_xclone.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t vec_dev;
static struct cdev vec_cdev;
static struct class *vec_class;

static int populate_vec_info(struct display_vec_info __user *display_vec_info)
{
	int ret = 0;
	struct display_vec_info *display_vec_info_k;
	pid_t *proc_list = NULL;
	struct list_vector_id *iter, *tmp;
  	bool is_empty = true;
  	int vct_iter = 0;
  	int tbl_iter = 0;
  	int sys_deny_ct = 0;
  	int sys_custom_ct = 0;
  	int proc_ctr = 0;
  	if (super_list_initialized == false) {
    	printk("super list not initialized\n");
    	return ret;
  	}

	display_vec_info_k = kmalloc(sizeof(struct display_vec_info), GFP_KERNEL);
	if (display_vec_info == NULL)
	{
		printk(KERN_ALERT "Insufficient memory\n");
		ret = -ENOMEM;
		goto out1;
	}

	
	ret = copy_from_user(display_vec_info_k, display_vec_info, sizeof(struct display_vec_info));
	if (ret != 0)
	{
		printk(KERN_ALERT "Copy from user failed for display_vec_info");
		ret = -EFAULT;
		goto out2;
	}
	list_for_each_entry_safe(iter, tmp, &super_list.list, list) {
		display_vec_info_k->vec_info_arr[vct_iter].vec_id = iter->vec_id;
		display_vec_info_k->vec_info_arr[vct_iter].refcnt = iter->vec->refcnt;
		sys_deny_ct = 0;
		sys_custom_ct = 0;
		proc_ctr = 0;
		for (tbl_iter = 0; tbl_iter < PPSV_MAX_SYSCALLS; tbl_iter++) {
	  		if (iter->vec->systabl[tbl_iter] > 0) {
	  			if (iter->vec->systabl[tbl_iter] == 1) {
	  				display_vec_info_k->vec_info_arr[vct_iter].syscall_denied_list[sys_deny_ct] = tbl_iter;
	  				sys_deny_ct++;
	  			} else {
	  				display_vec_info_k->vec_info_arr[vct_iter].syscall_custom_list[sys_custom_ct] = tbl_iter;
	  				sys_custom_ct++;
	  			}
	  		}
	  	}
	  	proc_list = vec_curr_proc_list(iter->vec);
	  	if(proc_list == NULL) {
	  		printk(KERN_ALERT "Unable to fetch process list for a vector!");
	  		ret = -EFAULT;
	  		goto out2;
	  	}
	  	for (tbl_iter = 0; tbl_iter < PPSV_MAX_PROCESSES; tbl_iter++) {
	  		if (proc_list[tbl_iter] != -1) {
  				display_vec_info_k->vec_info_arr[vct_iter].pid_list[proc_ctr] = proc_list[tbl_iter];
  				proc_ctr++;
  			}
  		}
  		display_vec_info_k->vec_info_arr[vct_iter].syscall_denied_cnt = sys_deny_ct;
		display_vec_info_k->vec_info_arr[vct_iter].syscall_custom_cnt = sys_custom_ct;
		display_vec_info_k->vec_info_arr[vct_iter].pid_cnt = proc_ctr;
	  	is_empty = false;
	  	vct_iter++;
	}
	  
	if (is_empty) {
	  printk("There is no registered vector");
	  ret = 0;
	  goto out2;
	}
	printk("\n");

	display_vec_info_k->vec_count = vct_iter;
	ret = copy_to_user(display_vec_info, display_vec_info_k, sizeof(struct display_vec_info));
	if (ret != 0)
	{
		printk(KERN_ALERT "Copy to user failed for display_vec_info\n");
		ret = -EFAULT;
	}	
	out2:
		kfree(display_vec_info_k);	
	out1:
		return ret;
}

int populate_process_vec_info(struct display_process_vec_arg __user *display_vec_info)
{
	int ret = 0;
	struct display_process_vec_arg *display_vec_info_k;
	pid_t *proc_list = NULL;
	struct list_vector_id *iter, *tmp;
  	bool is_empty = true;
  	int tbl_iter = 0;
  	int tbl_iter1 = 0;
  	int sys_deny_ct = 0;
  	int sys_custom_ct = 0;
  	int proc_ctr = 0;
  	pid_t target_pid = 0;
  	if (super_list_initialized == false) {
    	printk("super list not initialized\n");
    	return ret;
  	}

	display_vec_info_k = kmalloc(sizeof(struct display_process_vec_arg), GFP_KERNEL);
	if (display_vec_info == NULL)
	{
		printk(KERN_ALERT "Insufficient memory\n");
		ret = -ENOMEM;
		goto out1;
	}

	ret = copy_from_user(display_vec_info_k, display_vec_info, sizeof(struct display_process_vec_arg));
	if (ret != 0)
	{
		printk(KERN_ALERT "Copy from user failed for display_vec_info");
		ret = -EFAULT;
		goto out2;
	}

	target_pid = display_vec_info_k->pid;
	printk(KERN_ALERT "Target pid: %d", target_pid);
	list_for_each_entry_safe(iter, tmp, &super_list.list, list) {
		proc_list = vec_curr_proc_list(iter->vec);
	  	if(proc_list == NULL) {
	  		printk(KERN_ALERT "Unable to fetch process list for a vector!");
	  		ret = -EFAULT;
	  		goto out2;
	  	}
	  	for (tbl_iter1 = 0; tbl_iter1 < PPSV_MAX_PROCESSES; tbl_iter1++) {
	  		if (proc_list[tbl_iter1] != -1) {
  				 if (target_pid == proc_list[tbl_iter1]) {
  				 	display_vec_info_k->vec_info.vec_id = iter->vec_id;
  				 	display_vec_info_k->vec_info.refcnt = iter->vec->refcnt;
					for (tbl_iter = 0; tbl_iter < PPSV_MAX_SYSCALLS; tbl_iter++) {
				  		if (iter->vec->systabl[tbl_iter] > 0) {
				  			if (iter->vec->systabl[tbl_iter] == 1) {
				  				display_vec_info_k->vec_info.syscall_denied_list[sys_deny_ct] = tbl_iter;
				  				sys_deny_ct++;
				  			} else {
				  				display_vec_info_k->vec_info.syscall_custom_list[sys_custom_ct] = tbl_iter;
				  				sys_custom_ct++;
				  			}
				  		}
				  	}
				  	for (tbl_iter = 0; tbl_iter < PPSV_MAX_PROCESSES; tbl_iter++) {
	  						if (proc_list[tbl_iter] != -1) {
  								display_vec_info_k->vec_info.pid_list[proc_ctr] = proc_list[tbl_iter];
  								proc_ctr++;
  							}
  					}
					break;
  				 }
  			}
  		}
  		display_vec_info_k->vec_info.syscall_denied_cnt = sys_deny_ct;
		display_vec_info_k->vec_info.syscall_custom_cnt = sys_custom_ct;
		display_vec_info_k->vec_info.pid_cnt = proc_ctr;
	  	is_empty = false;
	}
	  
	if (is_empty) {
	  printk("There is no registered vector");
	  ret = 0;
	  goto out2;
	}
	printk("\n");
	ret = copy_to_user(display_vec_info, display_vec_info_k, sizeof(struct display_vec_info));
	if (ret != 0)
	{
		printk(KERN_ALERT "Copy to user failed for display_vec_info\n");
		ret = -EFAULT;
	}	
	out2:
		kfree(display_vec_info_k);	
	out1:
		return ret;
}

/**
 * Changes the "process vector" of running/suspended
 * process.
 */
static int update_process_vector(struct update_process_vec_arg __user *uarg) 
{
  int ret = 0;
  struct update_process_vec_arg *karg;
  struct task_struct *tsk;
  
  karg  = kmalloc(sizeof(*karg), GFP_KERNEL);
  if (karg == NULL)
  {
		printk(KERN_ALERT "Insufficient memory\n");
		ret = -ENOMEM;
		goto err;
	}
  
  ret =  copy_from_user(karg, uarg, sizeof(*uarg));
  if (ret != 0)
  {
		printk(KERN_ALERT "Copy from user failed for update_process_vector\n");
		ret = -EFAULT;
		goto free;
  }
  
  if (karg->pid < 0 || !is_registered_vector(karg->vec_id)) 
  {
		printk(KERN_ALERT "invalid input for update_process_vector\n");
    ret = -EINVAL;
    goto free;
  } 
  
  /* find the task_struct associated with pid nr */  
  tsk = find_task_by_vpid(karg->pid);
  if (tsk == NULL) 
  {
      ret = -ESRCH;
      goto free;
  }

  /* stop the process */ 
  ret = send_sig(SIGSTOP, tsk, 1);
  if (ret < 0)
  {
    VDBG;
    goto free;
  }
  printk("before update vec_id: %d\t vector:%p\n", tsk->vec.vec_id,
         tsk->vec.vector);
  /* detach old vector id from the process */
  if (tsk->vec.vec_id != 0)
  {
    ret = vec_detach_pid(tsk->vec.vector, tsk->pid);
    if(ret == -1) 
      printk(KERN_ALERT "Pid(%d) removal from vector failed\n",
             (int)tsk->pid);
    put_vec_refcount(tsk->vec.vector);
  }
  
  /* attach new vector id to the process */
  tsk->vec.vec_id = karg->vec_id;
  tsk->vec.vector = get_sys_vector(karg->vec_id);

  if (tsk->vec.vec_id != 0 )
  {
    ret = vec_attach_pid(tsk->vec.vector, tsk->pid);
    if(ret == -1)
      printk(KERN_ALERT "Pid(%d) attach to vector failed\n",
             (int)tsk->pid);
  }
  printk("after update vec_id: %d\t vector:%p\n", tsk->vec.vec_id,
         tsk->vec.vector);
//#define UPDATE_TEST
#ifdef UPDATE_TEST
  ssleep(5);
#endif
  /* resume the process */
  ret = send_sig(SIGCONT, tsk, 1); 

free:
  kfree(karg);
err:
  return ret;
}

static long vec_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	switch (cmd)
	{
		case VEC_IOCTL_DISPLAY_VEC_INFO:
			printk(KERN_ALERT "Calling populate function");
			ret = populate_vec_info((struct display_vec_info *) arg);
			return ret;
		case VEC_IOCTL_DISPLAY_PROCESS_VEC:
			ret = populate_process_vec_info((struct display_process_vec_arg *) arg);
			return ret;
    case VEC_IOCTL_UPDATE_PROCESS_VEC:
			printk(KERN_ALERT "Calling process vector function");
      ret = update_process_vector((struct update_process_vec_arg *)arg);
			return ret;
		default:
			printk(KERN_INFO "Invalid ioctl command\n");
			return 1;
	}
	return 0;
}

static struct file_operations vec_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = vec_ioctl
};

static int __init vec_ioctl_init(void)
{
	int ret;
	struct device *dev_ret;
	printk(KERN_ALERT "Init vec_ioctl");
	ret = alloc_chrdev_region(&vec_dev, FIRST_MINOR, MINOR_CNT, "vec_ioctl");
	if (ret < 0)
	{
		return ret;
	}

	cdev_init(&vec_cdev, &vec_fops);

	ret = cdev_add(&vec_cdev, vec_dev, MINOR_CNT);
	if (ret < 0)
	{
		return ret;
	}

	vec_class = class_create(THIS_MODULE, "vec");
	if (IS_ERR(vec_class))
	{
		cdev_del(&vec_cdev);
		unregister_chrdev_region(vec_dev, MINOR_CNT);
		return PTR_ERR(vec_class);
	}

	dev_ret = device_create(vec_class, NULL, vec_dev, NULL, DEVICE_FILE_NAME);
	if (IS_ERR(dev_ret))
	{
		printk("ioctl file was not created\n");
		class_destroy(vec_class);
		cdev_del(&vec_cdev);
		unregister_chrdev_region(vec_dev, MINOR_CNT);
	}

	return 0;
}

static void __exit vec_ioctl_exit(void)
{
	device_destroy(vec_class, vec_dev);
	class_destroy(vec_class);
	cdev_del(&vec_cdev);
	unregister_chrdev_region(vec_dev, MINOR_CNT);
}

module_init(vec_ioctl_init);
module_exit(vec_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CSE506P02");
MODULE_DESCRIPTION("vec ioctl");
