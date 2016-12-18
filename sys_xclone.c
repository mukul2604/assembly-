#include <linux/types.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "../hw3/sys_xclone.h"

/**
 * Each new module will register its vector to
 * global list. and the individual process
 * can get the relvant sys_vector by looking into 
 * this list using particular vector ID.
 */


/*TODO: protect it by lock or make it atomic*/ 
bool super_list_initialized = false;
EXPORT_SYMBOL(super_list_initialized);

/* Global vector list */
struct list_vector_id super_list;
EXPORT_SYMBOL(super_list);

/* Mutex lock on super list */
static DEFINE_MUTEX(super_lock);

/* Should be called for every module initialization */
void init_super_list(void) 
{
  if (super_list_initialized ==  false) {
    INIT_LIST_HEAD(&(super_list.list));
    super_list_initialized = true;
  }
}
EXPORT_SYMBOL(init_super_list);

/**
 * This will allocate a new node which contains
 * the vector id and pointer to the vector.
 * This node will be added to the super list.
 */
struct sys_vector **register_and_get_vector(int32_t vec_id)
{
  struct list_vector_id *node;
  
  if (super_list_initialized == false) {
    printk("list not initialized\n");
    return NULL;
  }

  node = (struct list_vector_id*) kmalloc(sizeof(*node), GFP_KERNEL);

  if (node ==  NULL) {
    printk("Unable to get memory for superlist node");
    return NULL;
  }

  node->vec_id = vec_id;
  node->vec = NULL;
  mutex_lock(&super_lock);
  list_add_tail(&node->list, &super_list.list);
  mutex_unlock(&super_lock);
  return &(node->vec);
}
EXPORT_SYMBOL(register_and_get_vector);

/**
 * removes the registered vector from the super list
 * if the refcount of the vector is zero.
 */
int unregister_vector(int32_t vec_id)
{
  struct list_vector_id *iter, *tmp;
  int32_t ref;

  if (vec_id <= 0) {
    VDBG;
    //BUG_ON("OOPS!!");
  }
 
  if (super_list_initialized == false) {
    VDBG;
    return -1;
  }

  mutex_lock(&super_lock);
  list_for_each_entry_safe(iter, tmp, &super_list.list, list) {
    if (iter->vec_id == vec_id) {
      ref = check_vec_refcount(iter->vec);
      if (ref) {
        VDBG;
        mutex_unlock(&super_lock);
        return -1;
      }
      list_del(&iter->list);
      mutex_unlock(&super_lock);
      kfree(iter);
      return 0;
    }
  }
  mutex_unlock(&super_lock);
  VDBG;
  return -1;
}
EXPORT_SYMBOL(unregister_vector);

bool is_registered_vector(int32_t vec_id) 
{
  struct list_vector_id *iter, *tmp;
 
  if (vec_id < 0) { 
    return false;
  } 
  /*
   * vector '0' is registered by
   * default.
   */
  if (vec_id == 0) {
    return true;
  }

  if (super_list_initialized == false) {
    return false;
  }

  mutex_lock(&super_lock);
  list_for_each_entry_safe(iter, tmp, &super_list.list, list) {
    if (iter->vec_id == vec_id) {
      mutex_unlock(&super_lock);
      return true;
    }
  }
  mutex_unlock(&super_lock);

  return false;
}
EXPORT_SYMBOL(is_registered_vector);

/**
 * Increment the refcount value of the vector.
 */

void get_vec_refcount(struct sys_vector *vec)
{
  bool status = false;
  int rf = 0;

  VDBG;
  if (vec != NULL) {
    mutex_lock(&(vec->refcnt_lock));
    vec->refcnt = vec->refcnt + 1;
    mutex_unlock(&(vec->refcnt_lock));
    if (vec->mod != NULL) {
       status = try_module_get(vec->mod);
       rf = module_refcount(vec->mod);
       printk(KERN_ALERT "[get] vec_addr: %p, status: %d, rf = %d", vec->mod, (int) status, rf);
     } else {
          printk(KERN_ALERT "vec->mod is NULL");
     }
  } else {
    printk(KERN_ALERT "[get_vec_refcount]: Vector struct found NULL!");
  }
}
EXPORT_SYMBOL(get_vec_refcount);

/**
 * Decrement the refcount value of the vector.
 */
void put_vec_refcount(struct sys_vector *vec)
{
  int rf = 0;
  VDBG;
  if (vec != NULL) {
    mutex_lock(&(vec->refcnt_lock));
    vec->refcnt = vec->refcnt - 1;
    mutex_unlock(&(vec->refcnt_lock));
    if (vec->mod != NULL) {
      printk(KERN_ALERT "vec->mod addr: %p", vec->mod);
      module_put(vec->mod);
      rf = module_refcount(vec->mod);
      printk(KERN_ALERT "[put] vec_addr: %p, rf = %d", vec->mod, rf);
    } else {
        printk(KERN_ALERT "vec->mod is NULL");
    }
  } else {
    printk(KERN_ALERT "[put_vec_refcount]: Vector struct found NULL!");
  }
}
EXPORT_SYMBOL(put_vec_refcount);

/**
 * return the refcount value of the vector.
 */
int32_t check_vec_refcount(struct sys_vector *vec)
{
  int32_t curr_refcnt;
  curr_refcnt = 0;
  VDBG;
  mutex_lock(&(vec->refcnt_lock));
  curr_refcnt = vec->refcnt;
  mutex_unlock(&(vec->refcnt_lock));
  return curr_refcnt;
}
EXPORT_SYMBOL(check_vec_refcount);

/**
  * Add process pid to the PPSV
  * Array index of attached pid is returned
  * Return value of -1 indicates failure.
  */
int vec_attach_pid(struct sys_vector *vec, pid_t pid)
{
  int pos = -1;
  int proc_iter = 0;
  if (vec == NULL) {
    printk(KERN_ALERT "Vector struct found NULL!");
    return pos;
  }
  mutex_lock(&(vec->proc_list_lock));
  for (proc_iter = 0; proc_iter < PPSV_MAX_PROCESSES; proc_iter++) {
    if (vec->proc_list[proc_iter] == -1) {
      vec->proc_list[proc_iter] = pid;
      pos = proc_iter;
      break;
    }
  }
  mutex_unlock(&(vec->proc_list_lock));
  return pos;
}
EXPORT_SYMBOL(vec_attach_pid);

/**
  * Remove process pid from the PPSV
  * Array index of removed pid is returned
  * Return value of -1 indicates failure.
  */
int vec_detach_pid(struct sys_vector *vec, pid_t pid)
{
  int pos = -1;
  int proc_iter = 0;
  if (vec == NULL) {
    printk(KERN_ALERT "Vector struct found NULL!");
    return pos;
  }
  mutex_lock(&(vec->proc_list_lock));
  for (proc_iter = 0; proc_iter < PPSV_MAX_PROCESSES; proc_iter++) {
    if(vec->proc_list[proc_iter] == pid) {
      vec->proc_list[proc_iter] = -1;
      pos = proc_iter;
      break;
    }
  }
  mutex_unlock(&(vec->proc_list_lock));
  return pos;
}
EXPORT_SYMBOL(vec_detach_pid);

/**
  * Get the current process list for the vector
  */
pid_t *vec_curr_proc_list(struct sys_vector *vec)
{
  static pid_t copy_proc_list[PPSV_MAX_PROCESSES];
  int proc_iter = 0;
  mutex_lock(&(vec->proc_list_lock));
  for (proc_iter = 0; proc_iter < PPSV_MAX_PROCESSES; proc_iter++) {
    copy_proc_list[proc_iter] = vec->proc_list[proc_iter];
  }
  mutex_unlock(&(vec->proc_list_lock));
  return copy_proc_list;
}
EXPORT_SYMBOL(vec_curr_proc_list);

/**
 * Get the sys_vector associated with vector id
 * from the super list and increment the refcount
 * on the vector.
 */
struct sys_vector *get_sys_vector(int32_t vector_id)
{
  struct list_vector_id *iter, *tmp;
  struct sys_vector *vector;

  if (vector_id == 0) {
    return NULL;
  }

  if (super_list_initialized == false) {
    return NULL;
  }

  /*
   * Get a sysvector
   * 1. loop through the super list with lock held
   * 2. increment refcount on vector with vector lock
   * 3. unlock the super lock
   */
  mutex_lock(&super_lock);
  list_for_each_entry_safe(iter, tmp, &super_list.list, list) {
    if (iter->vec_id == vector_id) {
      vector = iter->vec;
      get_vec_refcount(vector);
      mutex_unlock(&super_lock);
      if (iter->vec && iter->vec->refcnt <= 0) {
        VDBG;
      }
      return vector;
    }
  }
  mutex_unlock(&super_lock);
  VDBG;
  return NULL;
}
EXPORT_SYMBOL(get_sys_vector);


/* Should be used by IOCTL,  for now I am using directly.*/
void dump_vector_list(void) 
{
  struct list_vector_id *iter, *tmp;
  bool is_empty = true;
  int cnt_vct = 0;
  if (super_list_initialized == false) {
    printk("list not initialized\n");
    return;
  }

  printk("Dumping registered vector ids:\n");
  /**
   * this lock is probably not required since
   * we just dump the list which has no functional
   * impact but for uniform lock semantics, adding
   * lock/unlock for this too.
   */
  mutex_lock(&super_lock);
  list_for_each_entry_safe(iter, tmp, &super_list.list, list) {
    cnt_vct++;
    printk("%d\t%p\t%d\n", iter->vec_id, iter->vec, iter->vec->refcnt);
    is_empty = false;
  }
  mutex_unlock(&super_lock);
  
  if (is_empty) {
    printk("There is no registered vector");
  }
  printk("\n");
}
EXPORT_SYMBOL(dump_vector_list);
