ifneq ($(KERNELRELEASE),)
#obj-m := hello_world.o
obj-m := ds3231_jsalvador.o
#obj-$(CONFIG_ds3231) += ds3231_jsalvador.o
else
KDIR := $(HOME)/IMD/linux
all:
	$(MAKE) -C $(KDIR) M=$$PWD
endif
