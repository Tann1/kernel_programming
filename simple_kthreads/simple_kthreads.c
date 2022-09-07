#include <linux/init.h>
#include <linux/module.h>

#include <linux/string.h>	/* sprintf */
#include <linux/kthread.h> 	/* kernel thread */
#include <linux/delay.h>  	/* ssleep */
#include <linux/proc_fs.h>	/* proc file system struct */
#include <linux/slab.h>         /* GFP flags */
#include <asm/uaccess.h>	/* user access for copy_to_user */

MODULE_LICENSE("Dual BSD/GPL");

#define KTHREAD_SIZE 3
#define MESSAGE_SIZE 128 
#define KBUFF_SIZE 128

#define ENTRY_NAME "simple_kthread_counter"
#define PERMS 0644
#define PARENT NULL

static int counter = 0;
static int allow_read = 0;

static char *message = NULL;
static char kbuff[KBUFF_SIZE];

static struct task_struct *kthreads[KTHREAD_SIZE];
static struct proc_ops p_ops; 


int counter_run(void *data)
{

	char *curr_thread = (char *)data;
	printk(KERN_INFO "current kthread: %s\n", curr_thread);
	while(!kthread_should_stop()) {
		ssleep(1);
		counter += 1;
	}
	
	printk(KERN_INFO "The counter thread terminated\n");
	return counter;
}


int counter_proc_creation(struct inode *sp_inode, struct file *sp_file)
{
	printk(KERN_INFO "proc creation open\n");
	
	allow_read = 1; // allow read func to read 
	message = kmalloc(sizeof(char) * MESSAGE_SIZE, GFP_USER);
	if (message == NULL) {
		printk(KERN_ALERT "Err in %s\n", __FUNCTION__);	
		return -ENOMEM;
	}
	sprintf(message, "The counter is now at: %d\n", counter);
	return 0;
}


ssize_t counter_proc_read(struct file *sp_file, char __user *buffer, size_t size, loff_t *offset)
{
	int len = strlen(message);
	int unwritten_bytes = 0;

	printk(KERN_INFO "counter read called. len %d\n", len);
	
	allow_read = !allow_read;
	if (allow_read)
		return 0;

	unwritten_bytes = copy_to_user(buffer, message, len);
	return len - unwritten_bytes;
}

int counter_release(struct inode *sp_inode, struct file *sp_file)
{
	printk(KERN_INFO "proc called release\n");
	kfree(message);
	return 0;
}

static int counter_init(void)
{
	int err = 0;
	printk(KERN_INFO "/proc/%s create\n", ENTRY_NAME);

	for (int idx = 0; idx < KTHREAD_SIZE; ++idx) {
		sprintf(kbuff, "kthread-%d", idx);
		kthreads[idx] = kthread_run(counter_run, kbuff, kbuff);
		if (IS_ERR(kthreads[idx])) {
			printk(KERN_ALERT "err failed to create thread\n");
			err = 1;
		}
	}
	
	if (err)
		return -1;

	p_ops.proc_open = counter_proc_creation;
	p_ops.proc_read = counter_proc_read;
	p_ops.proc_release = counter_release;

	if (!proc_create(ENTRY_NAME, PERMS, PARENT, &p_ops)) {
		printk(KERN_ALERT "Err failed to create proc\n");
		remove_proc_entry(ENTRY_NAME, NULL);
		return -ENOMEM;
	}
	return 0;
}

static void counter_exit(void) 
{
	int ret;

	for (int idx = 0; idx < KTHREAD_SIZE; ++idx) {
		ret = kthread_stop(kthreads[idx]);
		if (ret != -EINTR)
			printk(KERN_ALERT "thread has stopped\n");
		remove_proc_entry(ENTRY_NAME, PARENT);
		printk("Removed /proc/%s\n", ENTRY_NAME);
	}
}

module_init(counter_init);
module_exit(counter_exit);	
