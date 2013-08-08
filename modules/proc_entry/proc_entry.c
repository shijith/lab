#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_BUFF_SIZE 100

MODULE_LICENSE("GPL");

static char *proc_entry_name = "test_proc", *parent_name = "shijith";
static struct proc_dir_entry *proc_entry, *parent, *leaf, *p1, *p2, *p3;

static int read_proc(char *buff,char **start, off_t offset, int count, int *eof, void *data)
{
	return sprintf(buff, "%s", (char *)data);
}

static int write_proc(struct file *file, const char *buff, unsigned long count, void *data)
{
	int ret;
	memset(data, 0, PROC_BUFF_SIZE);
	if(count > PROC_BUFF_SIZE)
		count = PROC_BUFF_SIZE;
	ret = copy_from_user((char *)data, buff, count);
	return count - ret;
}

static __init int proc_entry_init(void)
{
	parent = proc_mkdir(parent_name, NULL);
	proc_entry = create_proc_entry(proc_entry_name, 0666, parent);
	leaf = create_proc_entry("leaf", 0666,p3 = proc_mkdir("3", p2 = proc_mkdir("2", p1 = proc_mkdir("1", parent))));
	proc_entry->read_proc = read_proc;
	proc_entry->write_proc = write_proc;
	proc_entry->data = kmalloc(PROC_BUFF_SIZE, GFP_KERNEL);
	return 0;
}

static __exit void proc_entry_exit(void)
{
	kfree(proc_entry->data);
	remove_proc_entry(proc_entry_name, parent);
	remove_proc_entry("leaf", p3);
	remove_proc_entry("3", p2);
	remove_proc_entry("2", p1);
	remove_proc_entry("1", parent);
	remove_proc_entry(parent_name, NULL);
}

module_init(proc_entry_init);
module_exit(proc_entry_exit);
