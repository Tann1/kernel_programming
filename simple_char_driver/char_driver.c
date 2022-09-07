#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h> 		// char driver support 
#include <asm/uaccess.h> 	// use for put_user

MODULE_LICENSE("Dual BSD/GPL");

#define DEVICE_NAME "chardev"
#define BUF_LEN 1024
#define SUCCESS 0

static int major;
//static int device_open = 0;

static char kbuff[BUF_LEN];
static int num_bytes = 0;

int open(struct inode *p_inode, struct file *p_file)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}

ssize_t char_read(struct file *p_file, char __user *buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0;
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);

	if (offset == NULL) 
		return -1;
	if (*offset >= num_bytes)
		return 0;
	

	while (bytes_read < length && *offset < num_bytes) {
		put_user(kbuff[*offset], buffer + bytes_read);
		*offset += 1;
		bytes_read++;
	}
	
	return bytes_read;
}

ssize_t char_write(struct file *p_file, const char __user *buffer, size_t length, loff_t *offset)
{
	int bytes_wrote = 0;
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);

	if (offset == NULL)
		return -1;
	if (*offset >= BUF_LEN)
		return 0;
	
	while (bytes_wrote < length && *offset < BUF_LEN) {
		get_user(kbuff[*offset], buffer + bytes_wrote);
		*offset += 1;
		bytes_wrote++;
	}
	
	num_bytes = *offset; // *offset = original_offset + bytes_wrote
	
	return bytes_wrote;
}

int close(struct inode *p_inode, struct file *p_file)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}


struct file_operations fops =  {
	.owner = THIS_MODULE,
	.open = open,
	.read = char_read,
	.write = char_write,
	.release = close,
};


static int __init init_mod(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops); // register device and returns a major number associated with it
	
	if (major < 0) {
		printk(KERN_INFO "Registration failed with %d\n", major);
		return major;
	}

	printk(KERN_INFO "major number: %d\n", major);
	strncpy(kbuff, "Hello Worldn\n", BUF_LEN - 1); // set an original message just because for testing purposes
	num_bytes = strlen(kbuff);
	
	return SUCCESS;
}

static void __exit cleanup_mod(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "removed device with major number of: %d\n", major);
}



module_init(init_mod);
module_exit(cleanup_mod);
