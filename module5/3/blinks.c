#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>

MODULE_DESCRIPTION("Module illustrating the use of Keyboard LEDs with sysfs.");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sablin M.A.");

static struct kobject *example_kobject;
static struct timer_list my_timer;
static struct tty_driver *my_driver;
static int _kbledstatus = 0;
static int test = 3;

#define BLINK_DELAY   HZ/5
#define RESTORE_LEDS  0xFF
#define ATTR_NAME "systest"

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", test);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%du", &test);
    return count;
}

static struct kobj_attribute foo_attribute = __ATTR(test, 0664, foo_show, foo_store);

static void my_timer_func(struct timer_list *ptr)
{
    if (_kbledstatus == test)
        _kbledstatus = RESTORE_LEDS;
    else
        _kbledstatus = test;

    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, _kbledstatus);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
    int error = 0;

    printk(KERN_INFO "kbleds: loading\n");
    my_driver = vc_cons[fg_console].d->port.tty->driver;

    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    example_kobject = kobject_create_and_add(ATTR_NAME, kernel_kobj);
    if (!example_kobject)
        return -ENOMEM;

    error = sysfs_create_file(example_kobject, &foo_attribute.attr);
    if (error) {
        pr_debug("failed to create the foo file in /sys/kernel/systest \n");
    }

    return error;
}

static void __exit kbleds_cleanup(void)
{
    printk(KERN_INFO "kbleds: unloading...\n");
    del_timer(&my_timer);
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
    kobject_put(example_kobject);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);