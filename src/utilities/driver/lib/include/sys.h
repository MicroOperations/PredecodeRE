#ifndef _SYS_H_
#define _SYS_H_

#include "core.h"

extern struct kobj_attribute predecode_re_attr;

ssize_t predecode_re_show(struct kobject *kobj, struct kobj_attribute *attr, 
		                  char *buf);

#endif