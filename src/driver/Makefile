KERNEL_SRC := /lib/modules/$(shell uname -r)/build
SRC_DIR := .

LIB_DIR := lib
OBJ_FILES := $(LIB_DIR)/arch.o $(LIB_DIR)/utils.o $(LIB_DIR)/ioctl.o \
			$(LIB_DIR)/core.o

SRC_OBJ := driver.o
ALL_OBJS := $(SRC_OBJ) $(OBJ_FILES)
ccflags-y := -I$(SRC_DIR) -I$(LIB_DIR)/include

MODULE_NAME := predecode_re
obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(ALL_OBJS)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f $(ALL_OBJS)

.PHONY: all clean
