#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");

static int test_init(void)
{
	printk(KERN_ALERT "Hello world\n");
	return 0;
}

static void test_exit(void)
{
	printk(KERN_ALERT "Exiting\n");
}

module_init(test_init);
module_exit(test_exit);


