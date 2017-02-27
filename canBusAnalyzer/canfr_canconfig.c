/** 
 *  @defgroup canconfig CAN bus configuration
 *  @ingroup canbus
 *  @{
 *  @file canfr_canconfig.c
 *  @brief this module contain function to start, stop, and setting parameter for CAN bus channel on Beaglebone
 *  @author Deemarc Burakitbumrung
 *  @bug No known bugs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <sys/mman.h>
#include <rtdm/rtcan.h>
#include  <rtdk.h>
#include <canfr_canconfig.h>
#include <printActivation.h>

/**
 * @brief      Activate or start the CAN Bus so it is ready to be use
 *
 * @param[in]  itfName  telling which CAN channel to be activate base on busInterface value 
 *                      - Itf == ITF_RTCAN0    (==1) , start first channel 
 *                      - Itf == ITF_RTCAN1    (==2) , start second channel 
 *                      - Itf == ITF_RTCANALL  (==3) , start both channel
 * @param[in]  flags    The flags's value is for future used, don't have real meaning right
 *                      now so just send any int value
 *
 * @return     - return 0 uppon success      
 *             - CB_ERR_WRONG_BUS_TYPE, when itfName is not a valid value (the value is not in the range of 1-3) 
 *             - CB_ERROR, when it fail to start can channel(s)
 */
int canfr_canActivate(busInterface itfName, UINT8 flags)
{
    int ret = CB_ERROR;
    struct  ifreq ifr;
    char    ifname[16];
    //taking care only 3 fisrt bit (for CAN BUS)
    itfName = itfName & 3;

    /* Get CAN interface name */
    switch(itfName)
    {
        case ITF_RTCANALL:
            if (print_canconfig)
            {
                rt_printf("start up both CAN \n");   

            }
            // the function needs to set one itf at a time so we'll do it twice.
            strncpy(ifname, "rtcan0", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            //actual function that to the setting
            ret = canSetModeOnItf(ifr,CAN_MODE_START);
            if (ret != CB_ERROR)   
            {
                strncpy(ifname, "rtcan1", IFNAMSIZ);
                strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
                //actual function that to the setting
                ret =  canSetModeOnItf(ifr,CAN_MODE_START);    
            }
            break;
        case ITF_RTCAN0:
            if (print_canconfig)
            {
                rt_printf("start rtcan0\n");  

            }
            strncpy(ifname, "rtcan0", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

            ret =  canSetModeOnItf(ifr,CAN_MODE_START);    
            break;
        case ITF_RTCAN1:
            if (print_canconfig)
            {
                rt_printf("config on rtcan1 \n"); 

            }
            strncpy(ifname, "rtcan1", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            ret = canSetModeOnItf(ifr,CAN_MODE_START); 
            break;
        default:
            rt_printf("Unknown interface value ");
            ret =  CB_ERR_WRONG_BUS_TYPE;
            break;

    }

    return ret;


}

/**
 * @brief      Deactivate or stop the CAN Bus after finish using it
 *
 * @param[in]  itfName  telling which CAN channel to be deactivate base on busInterface value 
 *                      - Itf == ITF_RTCAN0    (==1) , start first channel 
 *                      - Itf == ITF_RTCAN1    (==2) , start second channel 
 *                      - Itf == ITF_RTCANALL  (==3) , start both channel
 * @param[in]  flags    The flags's value is for future used, don't have real meaning right
 *                      now so just send any int value
 *
 * @return     { description_of_the_return_value }
 */
int canfr_canDeactivate(busInterface itfName, UINT8 flags)
{
    int ret = CB_ERROR;
    struct  ifreq ifr;
    char    ifname[16];
    // taking care only 3 fisrt bit (for CAN BUS)
    itfName = itfName & 3;

    /* Get CAN interface name */
    switch(itfName)
    {
        case ITF_RTCANALL:
            if (print_canconfig)
            {
                rt_printf("stop up both CAN \n");   

            }
            // the function needs to set one itf at a time so we'll do it twice.
            strncpy(ifname, "rtcan0", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            //actual function that to the setting
            ret = canSetModeOnItf(ifr, CAN_MODE_STOP);
            if (ret != CB_ERROR)   
            {
                strncpy(ifname, "rtcan1", IFNAMSIZ);
                strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
                //actual function that to the setting
                ret =  canSetModeOnItf(ifr,CAN_MODE_STOP);    
            }
            break;
        case ITF_RTCAN0:
            if (print_canconfig)
            {
                rt_printf("start rtcan0\n");  

            }
            strncpy(ifname, "rtcan0", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

            ret =  canSetModeOnItf(ifr,CAN_MODE_STOP);    
            break;
        case ITF_RTCAN1:
            if (print_canconfig)
            {
                rt_printf("config on rtcan1 \n"); 

            }
            strncpy(ifname, "rtcan1", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            ret = canSetModeOnItf(ifr,CAN_MODE_STOP); 
            break;
        default:
            if(print_error)
                rt_printf(stderr, "Unknown interface value ");
            ret =  CB_ERR_WRONG_BUS_TYPE;
            break;

    }

    return ret;    
}


/**
 * @brief      set the baudrate of CAN bus channel
 *
 * @param[in]  itfName  telling which CAN channel to be deactivate base on busInterface value 
 *                      - Itf == ITF_RTCAN0    (==1) , start first channel 
 *                      - Itf == ITF_RTCAN1    (==2) , start second channel 
 *                      - Itf == ITF_RTCANALL  (==3) , start both channel
 * @param[in]  new_baudRate  The new baud rate
 *
 * @return     - return 0 uppon success      
 *             - CB_ERR_WRONG_BUS_TYPE, when itfName is not a valid value (the value is not in the range of 1-3) 
 *             - CB_ERROR, when it fail to start can channel(s)
 */
int canfr_canSetBit(busInterface itfName, UINT32 new_baudRate)
{
    char    ifname[16];
    
    struct  ifreq ifr;
    int ret = CB_ERROR;
    //taking care only 3 fisrt bit (for CAN BUS)
    itfName = itfName & 3;

    /* Get CAN interface name */
    switch(itfName)
    {
        case ITF_RTCANALL:
            strncpy(ifname, "rtcan0", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            ret = canSetbitRateOnItf(ifr,new_baudRate);
            if (print_canconfig)
            {
                rt_printf("config on both CAN \n"); 
                rt_printf("the return value of rtcan0 is %d\n", ret); 
            }
            
             if (ret != CB_ERROR)   
            {
                   
                strncpy(ifname, "rtcan1", IFNAMSIZ);
                strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
                ret =  canSetbitRateOnItf(ifr,new_baudRate);
                if(print_canconfig)
                    rt_printf("the return value of rtcan1 is %d\n", ret);     
            }
            
            break;
        case ITF_RTCAN0:
	        if (print_canconfig)
        	{
        		rt_printf("config on rtcan0\n");	

        	}
            strncpy(ifname, "rtcan0", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            ret =  canSetbitRateOnItf(ifr,new_baudRate);	
            break;
        case ITF_RTCAN1:
        	if (print_canconfig)
        	{
        		rt_printf("config on rtcan1 \n");	

        	}
            strncpy(ifname, "rtcan1", IFNAMSIZ);
            strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
            ret = canSetbitRateOnItf(ifr,new_baudRate);	
            break;
        default:
            if(print_error)
                rt_printf(stderr, "Unknown interface value ");

            ret =  CB_ERR_WRONG_BUS_TYPE;
            break;

    }

    return ret;
}


static int canSetModeOnItf(struct ifreq ifr, int newMode)
{
    can_mode_t *mode;
    int ret = CB_ERROR;
    int can_fd = -1;

    mode = (can_mode_t *)&ifr.ifr_ifru;
    *mode = newMode;

    //get rtcan interface handle (by finding it socket number)
    can_fd = rt_dev_socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_fd < 0) {
    if (print_error)
    {
        rt_printf("Cannot open RTDM CAN socket. Maybe driver not loaded? \n");
    }

    
    //printf(argArray, "Cannot open RTDM CAN socket. Maybe driver not loaded? \n");
    return CB_ERROR;
    }
    
    ret = rt_dev_ioctl(can_fd, SIOCSCANMODE, &ifr);
    if (ret) 
    {
            goto abort;
    }
    rt_dev_close(can_fd);
    return CB_SUCCESS;

 abort:
    rt_dev_close(can_fd);
    if (print_error)
    {
         rt_printf("setMode fail, abort the process");
    }
    return CB_ERROR;    
};
static int canSetbitRateOnItf(struct  ifreq ifr, UINT32 new_baudRate)
{
	can_baudrate_t *baudrate;
	int ret = CB_ERROR;
	int can_fd = -1;
	//get rtcan interface handle (by finding it socket number)
    can_fd = rt_dev_socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_fd < 0) {
    if (print_error)
    {
    	rt_printf("Cannot open RTDM CAN socket. Maybe driver not loaded? \n");
    }
    
    //printf(argArray, "Cannot open RTDM CAN socket. Maybe driver not loaded? \n");
    return CB_ERROR;
    }

    //point to specific rtcan channel
    ret = rt_dev_ioctl(can_fd, SIOCGIFINDEX, &ifr);
    if (ret) {
    	if (print_error)
	    {
	    	 rt_printf("Can't get interface index for %s, code = %d\n", ifr.ifr_name, ret);
	    }
   
    return CB_ERROR;
    }

    //set baudrate
    if (new_baudRate > 0)
    {
    	if (print_canconfig)
	    {
	    	rt_printf("new_baudRate: %lu\n", new_baudRate);
	    }
        baudrate = (can_baudrate_t *)&ifr.ifr_ifru;
        *baudrate = new_baudRate;
        ret = rt_dev_ioctl(can_fd, SIOCSCANBAUDRATE, &ifr);
        if (ret)
        {
            goto abort;
        }
    }

    rt_dev_close(can_fd);
    return CB_SUCCESS;

 abort:
    rt_dev_close(can_fd);
    if (print_error)
    {
    	 rt_printf("Config fail, abort the process");
    }
    return CB_ERROR;
}

/**
 * @}
 */
