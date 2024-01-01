#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>

#define MAX 1024 /*Max kFIFO size*/

static struct kfifo buf;
static struct dev_t devno;
static struct class *pclass;
static struct cdev *pchar_cdev;

static int major = 250;
static int kfifo_size = MAX;

static int pchar_open(struct inode *pinode, struct file *pfile);
static int pchar_close(struct inode *pinode, struct file *pfile);
static ssize_t pchar_read(struct file *pfile, char * __user ubuf, size_t ubuf_size, loff_t *fpos);
static ssize_t pchar_write(struct file *pfile, const char * __user ubuf, size_t ubuf_size, loff_t *fpos);


static struct file_operations pchar_fops = {

    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = ,
    .read = ,
    .write = 
};

static __init pchar_init(void)  
{
    int ret, minor;
    struct device *pdevice;

    printk(KERN_INFO "%s: pchar_init(CALLED).\n", THIS_MODULE->name);

    devno = MKDEV(major, 0);
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
        if(ret < 0) {
            printk(KERN_ERR "%s: device no. allocation(FAILED).\n", THIS_MODULE->name);
            return ret;
        }
    major = MAJOR(devno);
    minor = MINOR(devno);
    printk(KERN_INFO "%s: Device no. allocation (SUCCESS) Major = %d and Minor = %d.\n", THIS_MODULE->name, major, minor);


    return 0;
}