#include <linux/cdev.h> 
#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/irq.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/poll.h> 
 
static int device_open(struct inode *, struct file *); 
static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *); 
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *); 
 
#define SUCCESS 0 
#define DEVICE_NAME "mychdev"
#define BUF_LEN 80 
 
static int major;
enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN = 1, 
}; 
 
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED); 
static char msg[BUF_LEN];
static struct class *cls; 
static struct file_operations chardev_fops = { 
    .read = device_read, 
    .write = device_write, 
    .open = device_open, 
    .release = device_release, 
}; 
 
static int __init chardev_init(void) { 
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops); 
 
    if (major < 0) { 
        pr_alert("Registering char device failed with %d\n", major); 
        return major; 
    } 
    pr_info("I was assigned major number %d.\n", major); 
 
    cls = class_create(DEVICE_NAME); 
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME); 
    pr_info("Device created on /dev/%s\n", DEVICE_NAME); 
    return SUCCESS; 
} 
 
static void __exit chardev_exit(void){ 
    device_destroy(cls, MKDEV(major, 0)); 
    class_destroy(cls); 
    unregister_chrdev(major, DEVICE_NAME); 
} 
 
static int device_open(struct inode *inode, struct file *file){ 
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) 
        return -EBUSY; 
  
    try_module_get(THIS_MODULE); 
    return SUCCESS; 
} 
 
static int device_release(struct inode *inode, struct file *file){ 
    atomic_set(&already_open, CDEV_NOT_USED); 
    module_put(THIS_MODULE); 
    return SUCCESS; 
} 
 
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    int bytes_read = 0; 
    const char *msg_ptr = msg; 
    if (!*(msg_ptr + *offset)) { 
        *offset = 0; 
        return 0; 
    } 
    msg_ptr += *offset; 
    while (length && *msg_ptr) { 
        put_user(*(msg_ptr++), buffer++); 
        length--; 
        bytes_read++; 
    } 
    *offset += bytes_read; 
    return bytes_read; 
} 
 
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off){ 
    int num_bytes=0;
    memset (msg,0,BUF_LEN);
    while (len){
        get_user(*(msg+num_bytes),(buff+num_bytes));
        num_bytes++;
        len--;
    } 
    return num_bytes;
} 
 
module_init(chardev_init); 
module_exit(chardev_exit); 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sablin M.A.");
MODULE_DESCRIPTION("Character exchange module");