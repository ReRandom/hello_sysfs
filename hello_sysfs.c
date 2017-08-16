#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Roman Ponomarenko <R.E.P@yandex.ru>");

#define CLASS_NAME "hello_class_name"

static struct class* my_class;
static char* msg;

static ssize_t my_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t my_store(struct class *class, struct class_attribute *attr, const char* buf, size_t count);

CLASS_ATTR( xxx, 0644, &my_show, &my_store);

static ssize_t my_show(struct class *class, struct class_attribute *attr, char *buf)
{
	strcpy(buf, msg);
	size_t len = strlen(buf);
	printk(KERN_INFO "[hello_sysfs] read %ld bytes\n", len);
	return len;
}
static ssize_t my_store(struct class *class, struct class_attribute *attr, const char* buf, size_t count)
{
	msg = kmalloc(count+1, GFP_KERNEL);
	strncpy(msg, buf, count);
	msg[count] = 0;
	printk( KERN_INFO "[hello_sysfs] write %ld bytes\n", count);
	return count;
}

static int __init hello_init(void)
{
	int res;
	my_class = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR( my_class ) )
	{
		printk( KERN_ERR "[hello_sysfs] bad class create\n");
		return 1;
	}
	res = class_create_file(my_class, &class_attr_xxx);
	msg = kmalloc(20, GFP_KERNEL);
	if(msg == NULL)
	{
		printk( KERN_ERR "[hello_sysfs] failed to allocate memory\n");
		return 1;
	}
	sprintf(msg, "Hello, world!\n");
	printk(KERN_INFO "[hello_sysfs] init\n");
	return 0;
}

static void __exit hello_exit(void)
{
	kfree(msg);
	printk(KERN_INFO "[hello_sysfs] exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
