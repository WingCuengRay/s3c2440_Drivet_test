KERN_DIR?="/home/ray/work/system/transplant/linux-2.6.22.6"
#KERN_DIR?="/lib/modules/3.19.0-25-generic/build"

all	:	module leds_driver_test	

leds_driver_test:	leds_driver_test.c
	arm-linux-gcc -o $@ $^

module	:	s3c2440_leds.c
	$(MAKE) -C $(KERN_DIR) M=`pwd` modules

clean:
	rm -rf *.o *.ko *.mod.c .tmp_versions .*.cmd *.symvers leds_driver_test

obj-m += s3c2440_leds.o
