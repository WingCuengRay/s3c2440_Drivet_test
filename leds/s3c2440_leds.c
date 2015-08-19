/***************************************************************************************
 *	FileName					:   s3c24xx_leds.c	
 *	CopyRight					:
 *	ModuleName				:	
 *
 *	CPU							:   s3c2440
 *	RTOS						:
 *
 *	Create Data					:	2015/08/13
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:	led driver source code for JZ2440
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2015/8/13   	Ray												Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <asm/arch/regs-gpio.h>


/**************************************************************
*	Macro Define Section
**************************************************************/
#define DEVICE_NUM 123
#define DEVICE_NAME "s3c2440_leds"

#define LED_ON  1
#define LED_OFF 0


/**************************************************************
*	Struct Define Section
**************************************************************/


/**************************************************************
*	Prototype Declare Section
**************************************************************/
int s3c2440_leds_ioctl(struct inode *node,struct file *fs,unsigned int cmd,unsigned long led_num);
int s3c2440_leds_open(struct inode *node,struct file *fs);


/**************************************************************
*	Global Variable Declare Section
**************************************************************/
static struct file_operations s3c2440_fops = {
    .open = s3c2440_leds_open,
    .ioctl = s3c2440_leds_ioctl
};


unsigned long s3c2440_leds_pin[] = {
    S3C2410_GPF4,
    S3C2410_GPF5,
    S3C2410_GPF6
};

unsigned long s3c2440_leds_pin_cfg[] = {
    S3C2410_GPF4_OUTP,
    S3C2410_GPF5_OUTP,
    S3C2410_GPF6_OUTP
};


/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *  @name           void s3c2440_leds_register()
 *	@description    the register functions to kernel 
 *	@return         ret  -- int,    成功向内核注册驱动返回0,
 *	                                失败返回失败设备号的负数
 *  @notice
 */
static __init int s3c2440_leds_register()
{
   int ret;

   ret = register_chrdev(DEVICE_NUM,DEVICE_NAME,&s3c2440_fops);
   if(ret < 0)
   {
        printk(DEVICE_NAME "Error: Cannot find it!\n");
        return ret;
   } 

   printk(DEVICE_NAME "Initializes!\n");
   return 0;
}



/**
 *  @name           void s3c2440_leds_exit()
 *	@description    the unload functions to kernel 
 *  @notice
 */
void s3c2440_leds_exit()
{
    unregister_chrdev(DEVICE_NUM,DEVICE_NAME);
}


module_init(s3c2440_leds_register);
module_exit(s3c2440_leds_exit);



/**
 *  @name           int s3c2440_leds_open(struct inode *node,struct file *fs)
 *	@description    s3c2440初始化函数，在使用用户接口函数open()时，内核会自动调用此函数
 *	@param		    
 *	@return		
 *  @notice         参数的意义不用深究，在用户曾使用open()函数时，传递的参数会经过内核处理后传入这个函数
 */
int s3c2440_leds_open(struct inode *node,struct file *fs)
{
    int i;

    for(i=0; i<3; i++)
        s3c2410_gpio_cfgpin(s3c2440_leds_pin[i],s3c2440_leds_pin_cfg[i]);

    return 0;
}



/**
 *  @name           int s3c2440_leds_ioctl(struct inode *node,struct file *fs,
 *                                          unsigned int cmd,unsigned long led_num)
 *	@description    s3c2440 led的驱动控制函数，用户使用系统调用 ioctl()时内核会自动调用此函数
 *	@param			前两个参数由内核提供，后两个参数是系统调用 ioctl() 的最后两个参数
 *	                cmd -- unsigned int，LED_ON表示开灯，LED_OFF表示关灯
 *	                led_num    --  unsigned long, 表示操作的是第几盏灯
 *	@return		
 *  @notice
 */
int s3c2440_leds_ioctl(struct inode *node,struct file *fs,unsigned int cmd,unsigned long led_num)
{
    if(led_num > 3 || led_num < 1 || (cmd!=LED_ON && cmd!=LED_OFF) )
        return -EINVAL;

    led_num--;
    switch (cmd)
    {
    case LED_ON:
        s3c2410_gpio_setpin(s3c2440_leds_pin[led_num],0);
        break;
    case LED_OFF:
        s3c2410_gpio_setpin(s3c2440_leds_pin[led_num],1);
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */
 

 

 
 
