#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>  // For kmalloc/kfree
#include <linux/kobject.h>  // For sysfs

#define DEVICE_NAME "virt_sensor"
#define CLASS_NAME  "virt"

static dev_t dev_number;
static struct class* virt_class = NULL;
static struct cdev virt_cdev;

static struct kobject *virt_kobj;
static int mode = 0;
static char* leak_ptr = NULL;

static char sensor_data[] = "Virtual temperature: 28.5°C\n";

static ssize_t mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", mode);
}

static ssize_t mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%d", &mode);
    printk(KERN_INFO "virt_sensor: Mode set to %d\n", mode);
    return count;
}

static struct kobj_attribute mode_attribute = __ATTR(mode, 0660, mode_show, mode_store);

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "virt_sensor: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = copy_to_user(buffer, sensor_data, sizeof(sensor_data));
    if (error_count == 0) {
        return sizeof(sensor_data);
    } else {
        printk(KERN_ERR "virt_sensor: Failed to send data\n");
        return -EFAULT;
    }
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "virt_sensor: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};

static int __init virt_sensor_init(void) {
    alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    cdev_init(&virt_cdev, &fops);
    cdev_add(&virt_cdev, dev_number, 1);

    virt_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(virt_class, NULL, dev_number, NULL, DEVICE_NAME);

    virt_kobj = kobject_create_and_add("virt_sensor", kernel_kobj);
    sysfs_create_file(virt_kobj, &mode_attribute.attr);

    // Simulate memory leak (leak_ptr is never freed unless module exits)
    leak_ptr = kmalloc(1024, GFP_KERNEL);
    if (leak_ptr)
        printk(KERN_INFO "virt_sensor: Simulated memory allocation (leak)\n");

    printk(KERN_INFO "virt_sensor: Module loaded\n");
    return 0;
}

static void __exit virt_sensor_exit(void) {
    device_destroy(virt_class, dev_number);
    class_unregister(virt_class);
    class_destroy(virt_class);
    cdev_del(&virt_cdev);
    unregister_chrdev_region(dev_number, 1);

    sysfs_remove_file(virt_kobj, &mode_attribute.attr);
    kobject_put(virt_kobj);

    // Clean up memory
    if (leak_ptr)
        kfree(leak_ptr);

    printk(KERN_INFO "virt_sensor: Module unloaded\n");
}

module_init(virt_sensor_init);
module_exit(virt_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Virtual Sensor Driver with sysfs, memory leak demo, DT-ready");
MODULE_VERSION("2.0");
