#ifndef _IOCTL_H_
#define _IOCTL_H_

#include "utils.h"

extern struct file_operations predecode_re_ops;

long ioctl_handler(struct file *file, unsigned int cmd, unsigned long arg);

#endif