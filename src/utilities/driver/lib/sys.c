#include "include/sys.h"

extern struct predecode_re rawr;

struct kobj_attribute predecode_re_attr = 
    __ATTR(predecode_re, 0400, predecode_re_show, NULL);

ssize_t predecode_re_show(struct kobject *kobj, struct kobj_attribute *attr, 
		                  char *buf)
{
    return 0;
}