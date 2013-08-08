#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include "cdev_ioctl.h"
#define BUFF_SIZE 1024


MODULE_AUTHOR("Shijith T");
MODULE_DESCRIPTION("A character device driver");
MODULE_LICENSE("GPL");


static unsigned int dev_minor, dev_major, dev_count;
static char kbuff[BUFF_SIZE];
static dev_t dev;
static struct cdev cdev;
static struct class *cl;

static int cdev_open(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "opening character device: OK\n");
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
	int ret;
	if(*offp + count > BUFF_SIZE)
		count = BUFF_SIZE - *offp;
	if((ret = copy_from_user(kbuff + *offp, buff, count)))
		return -EFAULT;

	(*offp) += count;
	ret = count - ret;
	printk(KERN_INFO "wrote %d characters <off = %lld>\n", ret, *offp);
	return ret;
}

static int cdev_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "closing driver\n");
	return 0;
}

static int cdev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned long offset = arg;
	switch(cmd)
	{
		case CDEV_CLEAR:
			if(offset > BUFF_SIZE)
				offset = BUFF_SIZE;
			memset(kbuff + offset, 0, BUFF_SIZE - offset);
			break;
		default:
			return -ENOTTY;
			break;
	}

	return 0;
}

static struct file_operations cdev_fops = {
	open: cdev_open,
	read: cdev_read,
	write: cdev_write,
	release: cdev_release,
	ioctl: cdev_ioctl
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


	printk(KERN_ALERT "Initializing driver: OK\n");
	return 0;
}

static void __exit dev_dri_cleanup(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, dev_count);
	printk(KERN_ALERT "Module removed.\n");
}

module_init(dev_dri_init);
module_exit(dev_dri_cleanup);
