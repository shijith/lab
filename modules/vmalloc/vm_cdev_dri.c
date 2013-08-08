#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>

//#define BUFF_SIZE 1024
#define PROC_BUFF_SIZE 20
unsigned long int BUFF_SIZE;


MODULE_AUTHOR("Shijith T");
MODULE_DESCRIPTION("A character device driver");
MODULE_LICENSE("GPL");


static unsigned int dev_minor, dev_major, dev_count;
static char *kbuff;
static dev_t dev;
static struct cdev cdev;
static struct class *cl;

static char *proc_entry_name = "proc_entry";
static struct proc_dir_entry *proc_entry;

static int read_proc(char *buff,char **start, off_t offset, int count, int *eof, void *data)
{
	return sprintf(buff, "%s", (char *)data);
}

static int set_device_buff_size(void *data)
{

	if(BUFF_SIZE)
		vfree(kbuff);
	sscanf((char *)data, "%lu", &BUFF_SIZE);
	if(BUFF_SIZE <= 0)
		BUFF_SIZE = 32;
	kbuff = vmalloc(BUFF_SIZE);
	if(kbuff) {
//		printk(KERN_INFO "allocated memory of size %zu\n", ksize(kbuff));
//		memset(kbuff, 0, ksize(kbuff));
		memset(kbuff, 0, BUFF_SIZE);
	} else {
		printk(KERN_WARNING "error allocating memory of size %lu\n", BUFF_SIZE);
		BUFF_SIZE = 0;
		return -ENOMEM;
	}

	return BUFF_SIZE;
}

static int write_proc(struct file *file, const char *buff, unsigned long count, void *data)
{
	memset(data, 0, PROC_BUFF_SIZE);
	if(count > PROC_BUFF_SIZE)
		count = PROC_BUFF_SIZE;
	if( copy_from_user((char *)data, buff, count) )
		return -EFAULT;

	if( !(set_device_buff_size(data)) )
		return -ENOMEM;
	return count;
}

static int cdev_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "opening character device\n");
	return 0;
}

static ssize_t cdev_read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
	int ret;
	if(*offp < BUFF_SIZE) {
		if(*offp + count > BUFF_SIZE)
			count = BUFF_SIZE - *offp;

		if((ret = copy_to_user(buff, kbuff + *offp, count)))
			return -EFAULT;

		*offp += count;
		ret = count - ret;
		printk(KERN_INFO "read  %d characters <off = %lld>\n", ret, *offp);
		return ret;
	}
	return 0;
}

static ssize_t cdev_write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
	int ret = -1;
	if(BUFF_SIZE) {
		if(*offp + count > BUFF_SIZE)
			count = BUFF_SIZE - *offp;
		if((ret = copy_from_user(kbuff + *offp, buff, count)))
			return -EFAULT;

		(*offp) += count;
		ret = count - ret;
		printk(KERN_INFO "wrote %d characters <off = %lld>\n", ret, *offp);
	}
	return ret;
}

static int cdev_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "closing character device\n");
	return 0;
}

static struct file_operations cdev_fops = {
	open: cdev_open,
	read: cdev_read,
	write: cdev_write,
	release: cdev_release
};

static int __init dev_dri_init(void)
{
	int ret;
	char *dev_name = "chr_dev";
	dev_minor = 0;
	dev_count = 1;

	ret = alloc_chrdev_region(&dev, dev_minor, dev_count, dev_name); //connect dev_name and dev_driver with <maj,min> numbers
	dev_major = MAJOR(dev);
	printk(KERN_INFO "Device major number is %d\n", dev_major);
	
	if (ret < 0) {
		printk(KERN_WARNING "char_dev: problem getting \
				major number %d\n", dev_major);
		return ret;
	}
	
	cdev_init(&cdev, &cdev_fops);
	cdev.owner = THIS_MODULE;

	ret = cdev_add(&cdev, dev, dev_count);
	if(ret < 0) {
		printk(KERN_WARNING "unable to add character device\n");
		return ret;
	}

	cl = class_create(THIS_MODULE, "char_dri");
	device_create(cl, NULL, dev, NULL, dev_name);

	if(BUFF_SIZE == 0) {

	}
	
	proc_entry = create_proc_entry(proc_entry_name, 0666, NULL);
	proc_entry->read_proc = read_proc;
	proc_entry->write_proc = write_proc;
	proc_entry->data = kmalloc(PROC_BUFF_SIZE, GFP_KERNEL);

	printk(KERN_INFO "Initializing driver: OK\n");
	return 0;
}

static void __exit dev_dri_cleanup(void)
{
	kfree(proc_entry->data);
	vfree(kbuff);
	remove_proc_entry(proc_entry_name, NULL);
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, dev_count);
	printk(KERN_INFO "Module removed.\n");
}

module_init(dev_dri_init);
module_exit(dev_dri_cleanup);
