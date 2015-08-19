/***************************************************************************************
 *	FileName					:   leds_driver_test.c	
 *	CopyRight					:
 *	ModuleName				:	
 *
 *	CPU							:
 *	RTOS						:
 *
 *	Create Data					:	2015/08/13
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:	This module is tested for s3c2440_leds driver
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2015/8/13		Ray									Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


/**************************************************************
*	Macro Define Section
**************************************************************/
#define DEVICE_NAME "leds_driver_test"

#define LED_ON  1
#define LED_OFF 0


/**************************************************************
*	Struct Define Section
**************************************************************/


/**************************************************************
*	Prototype Declare Section
**************************************************************/


/**************************************************************
*	Global Variable Declare Section
**************************************************************/


/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/
void usage(char *str)
{
    printf("Usage:\n");
    printf("   %s <led_no> <on|off>\n",str);
    printf("   led_no = 1,2,3\n");
}


int main(int argc,char **argv)
{
    int fd;
    int led_num;

    if(argc != 3)
        goto err;

    fd = open("/dev/leds",0); 
    if(fd < 0)
    {
        printf("Cannot open device.\n");
        return -1;
    }

    led_num = strtoul(argv[1], 0, 0);
    if(!strcmp(argv[2],"on"))
        ioctl(fd,LED_ON,led_num);
    else if(!strcmp(argv[2],"off"))
        ioctl(fd,LED_OFF,led_num);
    else
        goto err;

    return 0;

err:
    usage(DEVICE_NAME);
    return -1;
}

/**
 *  @name
 *	@description
 *	@param			
 *	@fdurn		
 *  @notice
 */
 

 

 
