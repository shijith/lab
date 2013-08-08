#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#define TIME 5*HZ

MODULE_AUTHOR("Shijith T");
MODULE_DESCRIPTION("standered timer example");
MODULE_LICENSE("GPL");

static struct timer_list timer;

static void foo(unsigned long val)
{
	printk(KERN_INFO "%lums timer expired\n", val);
}

static int __init timer_init(void)
{
//	timer.function = foo;
//	timer.data = TIME;
//	init_timer(&timer);
	setup_timer(&timer, foo, TIME);
	mod_timer(&timer, jiffies + msecs_to_jiffies(TIME));
	printk(KERN_INFO "started timer with %dms\n", TIME); 
	return 0;
}

static void __exit timer_exit(void)
{
	printk(KERN_INFO "removed timer\n");
	del_timer(&timer);
}

module_init(timer_init);
module_exit(timer_exit);
