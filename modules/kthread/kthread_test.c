#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/timer.h>

#define TIMEOUT HZ/100
#define THREAD_NUM 10

#define DPRINT(fmt,args ...) printk(KERN_INFO "%s,%i:" fmt "\n", __FUNCTION__, __LINE__,##args);

static struct task_struct *ts_array[THREAD_NUM];
static int a[THREAD_NUM], b[THREAD_NUM];

int thread_fun(void *id)
{
	int tid = *(int *)id;
	printk(KERN_INFO "in thread %d\n", tid);
	a[tid] += b[tid];
//	printk(KERN_INFO "state = %ld\n", current->state);
//	set_current_state(TASK_INTERRUPTIBLE);
//	printk(KERN_INFO "state = %ld\n", current->state);
//	if(tid >= 0)
//		schedule_timeout(HZ*tid);
	set_current_state(TASK_INTERRUPTIBLE);
	while(! kthread_should_stop()) {
		schedule();
		set_current_state(TASK_INTERRUPTIBLE);
	}
	set_current_state(TASK_RUNNING);
	printk(KERN_INFO "exiting thread %d\n", tid);
	return 0;
}

static int __init thread_init(void)
{
	int i;
	char thread_name[10] = {'\0'};
	DPRINT(" Initializing\n");
	for(i = 0; i < THREAD_NUM; i++) {
		a[i] = b[i] = i;
		sprintf(thread_name, "thread_%d", i);
		ts_array[i] = kthread_run(thread_fun, &i, thread_name);
	}
	return 0;
}

static void __exit thread_exit(void)
{
	int i;
	for(i = 0; i < THREAD_NUM; i++) {
		printk(KERN_INFO "%d ", a[i]);
		kthread_stop(ts_array[i]);
	}

	DPRINT(" exiting");
}

module_init(thread_init);
module_exit(thread_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shijith T");
