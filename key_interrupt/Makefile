KERNELDIR?="~/work/system/transplant/linux-2.6.22.6"
#KERNELDIR?="/lib/modules/3.19.0-25-generic/build"


all:	module key_test

key_test:	key_test.c
	arm-linux-gcc -o $@ $^

module:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules

clean:
	rm -rf *.o *.ko *.mod.c .tmp_versions .*.cmd *.symvers leds_driver_test

obj-m += s3c2440_key.o
