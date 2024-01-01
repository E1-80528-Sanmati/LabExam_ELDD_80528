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

    if(kfifo_size > MAX)    /*VERIFY KFIFO size*/    
    {   
        printk(KERN_ERR "%s: Invalid kFIFO size ( Must be <= %d ).\n", THIS_MODULE->name, MAX);
        return -EINVAL;
    }

    devno = MKDEV(major, 0);
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
        if(ret < 0) {
            printk(KERN_ERR "%s: device no. allocation(FAILED).\n", THIS_MODULE->name);
            return ret;
        }
    major = MAJOR(devno);
    minor = MINOR(devno);
    printk(KERN_INFO "%s: Device no. allocation (SUCCESS) Major = %d and Minor = %d.\n", THIS_MODULE->name, major, minor);

    pclass = class_create(THIS_MODULE, "pchar_class");
        if(IS_ERR(pclass))  {
            printk(KERN_ERR "%s: class_create (FAILED).\n", THIS_MODULE->name);
            unregister_chrdev_region(devno, 1);
            return -1;
        }
    printk(KERN_INFO "%s: class_create (SUCCESS) device class created.\n", THIS_MODULE->name);

    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d", 0);
        if(IS_ERR(pdevice)) {
            printk(KERN_ERR "%s: device_create (FAILED).\n", THIS_MODULE->name);
            class_destroy(pclass);
            unregister_chrdev_region(devno, 1);
            return -1;
        }
    printk(KERN_INFO "%s: device_create (SUCCESS) device file created.\n", THIS_MODULE->name);

    cdev_init(&pchar_cdev, &pchar_fops);
    pchar_cdev->owner = THIS_MODULE;
    ret = cdev_add(&pchar_cdev, devno, 1);
        if( ret < 0)    {
            printk(KERN_ERR "%s: cdev_add (FAILED) device .\n", THIS_MODULE->name);
            device_destroy(pclass, devno);
            class_destroy(pclass);
            unregister_chrdev_region(devno, 1);
            return -1;
        }
    printk(KERN_INFO "%s: cdev_add (SUCCESS) device added in KERNEL dB.\n", THIS_MODULE->name);

    ret = kfifo_alloc(&buf, kfifo_size, GFP_KERNEL);
        if(ret < 0) {
            printk(KERN_ERR "%s: kfifo_alloc (FAILED).\n", THIS_MODULE->name);
            cdev_del(&pchar_cdev);
            device_destroy(pclass, devno);
            class_destroy(pclass);
            unregister_chrdev_region(devno, 1);
            return ret;
        }
    printk(KERN_INFO "%s: kfifo_alloc (SUCCESS) allocated kFIFO.\n", THIS_MODULE->name);


    return 0;
}

static __exit void pchar_exit(void)
{
    printk(KERN_INFO "%s: pchar_exit (CALLED).\n", THIS_MODULE->name);

}