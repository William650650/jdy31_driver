ARCH ?= arm
CROSS_COMPILE ?= arm-none-linux-gnueabihf-
KERNEL_DIR ?= ~/project/linux-6.6.65/
PWD := $(shell pwd)

MODULE_NAME := jdy_31
SRC_FILES := jdy31_serdev.c at_cmd.c jdy31_sysfs.c
OBJ_FILES := $(SRC_FILES:.c=.o)

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-y := $(OBJ_FILES)

all:
	$(MAKE) -C $(KERNEL_DIR) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} M=$(PWD) clean
	rm -f modules.order Module.symvers .*.cmd *.mod.c

install:
	cp $(MODULE_NAME).ko ~/workspace/nfs/lib/modules/

uninstall:
	sudo rmmod $(MODULE_NAME)

.PHONY: all clean install uninstall
