#include "include/utils.h"

struct chardev *alloc_chardev(char *device_name, struct file_operations *fops,
                              char *class_name, struct device *parent,
                              void *drvdata, char *chardev_name)
{
    struct chardev *chardev = kzalloc(sizeof(struct chardev), GFP_KERNEL);
    if (!chardev)
        return ERR_PTR(-ENOMEM);

    void *ptr_err = NULL;
    int err = alloc_chrdev_region(&chardev->dev, 0, 1, device_name);
    if (err < 0) {
        ptr_err = ERR_PTR(err);
        goto alloc_chrdev_region_failed;
    }

    cdev_init(&chardev->cdev_node, fops);

    err = cdev_add(&chardev->cdev_node, chardev->dev, 1);
    if (err < 0) {
        ptr_err = ERR_PTR(err);
        goto cdev_add_failed;
    }

    /* little kernel version check so that this shit can 
       compile both on my wsl2 which is on kernel 6.6 and 
       on my debian which is on kernel 6.1 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    chardev->device_class = class_create(class_name);
#else
    chardev->device_class = class_create(THIS_MODULE, class_name);
#endif

    if (IS_ERR(chardev->device_class)) {
        ptr_err = chardev->device_class;
        goto class_create_failed;
    }

    chardev->device_node = device_create(chardev->device_class, parent,
                                         chardev->dev, drvdata, chardev_name);

    if (IS_ERR(chardev->device_node)) {
        ptr_err = chardev->device_node;
        goto device_create_failed;
    }

    return chardev;

device_create_failed:
    class_destroy(chardev->device_class);

class_create_failed:
    cdev_del(&chardev->cdev_node);

cdev_add_failed:
    unregister_chrdev_region(chardev->dev, 1);

alloc_chrdev_region_failed:
    kfree(chardev);
    return ptr_err;
}

void free_chardev(struct chardev *chardev)
{
    if (!chardev)
        return;

    device_destroy(chardev->device_class, chardev->dev);
    class_destroy(chardev->device_class);
    cdev_del(&chardev->cdev_node);
    unregister_chrdev_region(chardev->dev, 1);
    kfree(chardev);
    chardev = NULL;
}