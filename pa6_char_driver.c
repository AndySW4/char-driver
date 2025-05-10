#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVNAME "pa6_char_device"
#define BUFSIZE 900

static char *dev_buf;
static int opens, closes;

static int dev_open(struct inode *i, struct file *f) {
    opens++;
    printk(KERN_INFO "Opened %d\n", opens);
    return 0;
}

static int dev_release(struct inode *i, struct file *f) {
    closes++;
    printk(KERN_INFO "Closed %d\n", closes);
    return 0;
}

static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *pos) {
    if (*pos >= BUFSIZE) return 0;
    if ((*pos + len) > BUFSIZE) len = BUFSIZE - *pos;
    if (copy_to_user(buf, dev_buf + *pos, len)) return -EFAULT;
    *pos += len;
    printk(KERN_INFO "Read %zu @ %lld\n", len, *pos);
    return len;
}

static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *pos) {
    if (*pos >= BUFSIZE) return 0;
    if ((*pos + len) > BUFSIZE) len = BUFSIZE - *pos;
    if (copy_from_user(dev_buf + *pos, buf, len)) return -EFAULT;
    *pos += len;
    printk(KERN_INFO "Wrote %zu @ %lld\n", len, *pos);
    return len;
}

static loff_t dev_llseek(struct file *f, loff_t off, int whence) {
    loff_t newpos;
    switch (whence) {
    case SEEK_SET: newpos = off; break;
    case SEEK_CUR: newpos = f->f_pos + off; break;
    case SEEK_END: newpos = BUFSIZE + off; break;
    default: return -EINVAL;
    }
    if (newpos < 0) newpos = 0;
    if (newpos > BUFSIZE) newpos = BUFSIZE;
    f->f_pos = newpos;
    printk(KERN_INFO "Seek %lld\n", newpos);
    return newpos;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
    .llseek  = dev_llseek,
};

static int __init dev_init(void) {
    int r;
    dev_buf = kmalloc(BUFSIZE, GFP_KERNEL);
    if (!dev_buf) return -ENOMEM;
    memset(dev_buf, 0, BUFSIZE);
    r = register_chrdev(511, DEVNAME, &fops);
    if (r < 0) {
        kfree(dev_buf);
        return r;
    }
    printk(KERN_INFO "Loaded (major=511)\n");
    return 0;
}

static void __exit dev_exit(void) {
    unregister_chrdev(511, DEVNAME);
    kfree(dev_buf);
    printk(KERN_INFO "Unloaded\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Wood");
MODULE_DESCRIPTION("PA6 Driver");
