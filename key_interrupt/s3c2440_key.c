/***************************************************************************************
 *	FileName					:   s3c2440_key.c
 *	CopyRight					:
 *	ModuleName				    :   s3c2440的按键驱动程序
 *
 *	CPU							:
 *	RTOS						:
 *
 *	Create Data					:	2015/08/17
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:   利用了外部中断，对按键的次数进行检测	
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2015/8/17  		Ray									Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <asm-arm/arch-s3c2410/irqs.h>
#include <asm/uaccess.h>


/**************************************************************
*	Macro Define Section
**************************************************************/
#define DEVICE_MAJOR 140
#define DEVICE_NAME "s3c2440_key"


/**************************************************************
*	Struct Define Section
**************************************************************/
struct Key_Interrupt_type
{
    unsigned int irq;
    irqreturn_t handler;
    unsigned long irqflags;
    const char *devname;
    int count;
};

struct char_dev
{
    struct cdev cdev;
};


/**************************************************************
*	Prototype Declare Section
**************************************************************/
static int s3c2440_key_open(struct inode *inode, struct file *fs);
static int s3c2440_key_relase(struct inode *inode, struct file *fs);
static ssize_t s3c2440_key_read(struct file *fs,char __user *buff, size_t size, loff_t *offset);
static irqreturn_t button_press(int irq, void *dev_id);


/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct Key_Interrupt_type key[] = {
    {IRQ_EINT0, button_press, IRQF_TRIGGER_FALLING, "KEY1",0},
    {IRQ_EINT2, button_press, IRQF_TRIGGER_FALLING, "KEY2",0},
    {IRQ_EINT11, button_press, IRQF_TRIGGER_FALLING, "KEY3",0},
    {IRQ_EINT19, button_press, IRQF_TRIGGER_FALLING, "KEY4",0}
};

struct file_operations fops = {
    .owner = THIS_MODULE, 
    .open = s3c2440_key_open,
    .read = s3c2440_key_read,
    .release = s3c2440_key_relase
};

struct char_dev s3c2440_key_dev;

int has_pressed = 0;


static DECLARE_WAIT_QUEUE_HEAD(button_waitq);   //声明一个进程的队列，名字叫 button_waitq

/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/

/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */
 

/**
 *  @name           static __init int s3c2440_key_init(void)
 *	@description    按键中断驱动程序的注册函数
 *	@return         注册成功返回0,
 *	                注册失败返回一个负数值。
 *  @notice         使用早期的注册函数
 */
static __init int s3c2440_key_init(void)
{
    int ret;

//  ret = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
    ret = register_chrdev_region(MKDEV(DEVICE_MAJOR,0),1,DEVICE_NAME);
    if(ret)
    {
        printk(DEVICE_NAME " Cannot register key driver!\n");
        return ret;
    }

    //注册字符设备
    //这里使用新版的方法，利用 cdev_init和 cdev_add 进行注册。老式方式直接使用 register_chrdev 注册
    cdev_init(&s3c2440_key_dev.cdev,&fops);
    s3c2440_key_dev.cdev.owner = THIS_MODULE;
    ret = cdev_add(&s3c2440_key_dev.cdev, MKDEV(DEVICE_MAJOR,0), 1);
    if(ret)
    {
        printk(KERN_NOTICE "Error adding key devices!\n");
        return ret;
    }
/**/

    printk(DEVICE_NAME " register sucessfully!\n");
    return 0;
}


/**
 *  @name           static __exit void s3c2440_key_exit(void)
 *	@description    按键驱动程序的卸载函数
 *  @notice         使用旧版的卸载程序
 */
static __exit void s3c2440_key_exit(void)
{
//  unregister_chrdev(DEVICE_MAJOR, DEVICE_NAME);
    unregister_chrdev_region(MKDEV(DEVICE_MAJOR,0),1);
}

module_init(s3c2440_key_init);
module_exit(s3c2440_key_exit);



/**
 *  @name           int s3c2440_key_open(struct inode *inode, struct file *fs)
 *	@description    打开设备驱动程序，进行设备驱动程序的初始化。
 *	                在按键中断程序中为 对每个外部中断进行注册，包括中断源，中断函数，中断名称等
 *	@param			inode   -- struct inode *,内核使用inode结构在内部表示文件，所以这个参数表示目前打开的文件节点
 *	                fs      -- struct file *,暂时不了解其作用，只知道在用户使用系统调用open()时系统提供
 *  @notice         成功返回0,
 *                  失败返回一个负数值。
 */
int s3c2440_key_open(struct inode *inode, struct file *fs)
{
    int i;
    int err = 0;
    int *ptr = NULL;
    
    *ptr = 1;

    //为4个外部中断注册，包括中断源，中断函数，中断名称等
    for(i=0; i<4; i++)
    {
        err = request_irq(key[i].irq, key[i].handler, key[i].irqflags, key[i].devname, &key[i].count);
        if(err)
            break;
    }

    if(err)
    {
        printk("Error: Register interrutpt failure!\n");
        i--;
        for(; i>=0; i--)
            free_irq(key[i].irq, &key[i].count);
    }


    return 0;
}



/**
 *  @name           int s3c2440_key_relase(struct inode *inode, struct file *fs)
 *	@description    驱动释放函数,具体工作为卸载中断处理函数
 *	@param			inode   -- struct inode *,内核使用inode结构在内部表示文件，所以这个参数表示目前打开的文件节点
 *	                fs      -- struct file *,暂时不了解其作用，只知道在用户使用系统调用open()时系统提供
 *	@return		    同上
 */
int s3c2440_key_relase(struct inode *inode, struct file *fs)
{
    int i;

    for(i=3; i>=0; i--)
        free_irq(key[i].irq, (void *)&key[i].count);

    return 0;
}


/**
 *  @name           irqreturn_t button_press(int irq, void *dev_id)
 *	@description    按键中断处理函数，具体操作为将对应中断的计数变量加1,并唤醒休眠的进程
 *	@param		    irq     --  int,中断号
 *	                dev_id  --  void *, 传递给中断处理函数的参数。
 *	                                    这个参数为在注册中断时传递给 request_irq()的最后一个参数 dev_id.
 *	@return		    irqreturn_t     --  中断返回专用的类型
 *  @notice
 */
irqreturn_t button_press(int irq, void *dev_id)
{
    int *count = (int *)dev_id;

    (*count)++;
    has_pressed = 1;                        //满足唤醒的条件1
    wake_up_interruptible(&button_waitq);   //唤醒休眠的进程

    return IRQ_HANDLED;
}


/**
 *  @name           ssize_t s3c2440_key_read(struct file *fs,char __user *buff, size_t size, loff_t *offset)
 *	@description    驱动读取函数。将中断记录的按键次数复制到用户区，给应用程序使用    
 *	@param		    fs      --  struct file *,文件指针，具体暂时不清楚
 *	                buff    --  char __user *,用户空间的缓存去，要复制的终点站
 *	                size    --  size_t,复制数据的大小
 *	                offset  --  loff_t *,指向 长偏移量类型 的指针，指明用户在文件中进行存取操作的位置。
 *	@return		    暂时不清楚...
 *  @notice
 */
ssize_t s3c2440_key_read(struct file *fs,char __user *buff, size_t size, loff_t *offset)
{
    int i;
    int err;

    wait_event_interruptible(button_waitq, has_pressed);    //使进程休眠，知道has_pressed被中断函数置为1且进程被唤醒
    has_pressed = 0;
    
    for(i=0; i<size; i++)
    {
        err = put_user(key[i].count,buff+sizeof(key[i].count)*i);   //put_user()宏的第一个参数是值，而不是变量的地址！第二个参数才是用户空间的地址
//      memset((void *)&key[i].count, 0, sizeof(key[i].count));
        if(err)
            return err;
    }

    return 0;
}


