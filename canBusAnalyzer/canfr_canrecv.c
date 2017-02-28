/**
 *  @defgroup canrecv CAN bus receive
 *  @ingroup canbus 
 *  @{
 *  @file canfr_canrecv.c
 *  @brief this module contain function to receive CAN bus message also with trasmited message and send back to CANNIBESSA
 *  @author Deemarc Burakitbumrung
 *  @bug No known bugs.
 *  
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/queue.h>
#include <canfr_canrecv.h>
#include <rtdk.h>
#include <rtdm/rtcan.h>
#include <printActivation.h>
#define NTASKS 2
#define maxBufferNum = 2


#define MAX_MESSAGE_LENGTH 40
#define SEM_INIT 1       /* Initial semaphore count */
#define SEM_MODE S_FIFO  /* Wait by FIFO order */

RT_SEM sem_desc;
//RT_TASK task_struct[NTASKS];

RT_TASK rtcan0MsgRecv_task;
RT_TASK rtcan1MsgRecv_task;
RT_TASK logSend_task;
//RT_QUEUE canMsgRecvQ;
RT_QUEUE* rtcan0BufferPrt;
RT_QUEUE* rtcan1BufferPrt;
nanosecs_abs_t startTime =0;


static int rtcan0_socket = -1;
static int rtcan1_socket = -1;
struct sockaddr_can rtcan0Recv_addr;
struct sockaddr_can rtcan1Recv_addr;
double velocity=0;

static int cleanup(void)
{
	int ret =  CB_ERROR;
	int killTask =0;

	if(print_canrecv)
		rt_printf("Canfr canrecv is Cleaning up...\n");


	killTask = rt_task_delete(&rtcan0MsgRecv_task);
	if(killTask)
	{
		if(print_error)
			rt_printf("Error killTask rtcan0 \n");
		ret =  CB_ERROR;
		return ret;
	}

	rt_task_delete(&rtcan1MsgRecv_task);
	if(killTask)
	{
		if(print_error)
			rt_printf("Error killTask rtcan1 \n");
		ret =  CB_ERROR;
		return ret;
	}

	if(rtcan0_socket>0)
	{
		ret = rt_dev_close(rtcan0_socket);
		if(ret)
		{
			if(print_error)
				rt_printf( "Error rt_can0_close: %s\n", strerror(-ret));
			ret =  CB_ERROR;
			return ret;
		}
	}
	if(rtcan1_socket>0)
	{
		ret = rt_dev_close(rtcan1_socket);
		if(ret)
		{
			if(print_error)
			rt_printf("Error rt_can1_close: %s\n", strerror(-ret));
			ret =  CB_ERROR;
			return ret;
		}
	}
	ret =  CB_SUCCESS;
	return ret;


}


static void rtcan0MsgRecv_func(void *arg)
{
	int i, ret, count = 1;
    uint8_t msgFlag =0;
    struct can_frame frame;
    struct sockaddr_can addr;
    struct msghdr msg;
    struct iovec iov;
    canRxStruct rxCan0Msg;
	int retval = 0;
	
	nanosecs_abs_t timestamp = 0;
	// unsigned char packet[32];



    msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = (void *)&addr;
	msg.msg_namelen = sizeof(struct sockaddr_can);
	msg.msg_control = (void *)&timestamp;
	msg.msg_controllen = sizeof(nanosecs_abs_t);
	msg.msg_flags = (void *)&msgFlag;

	 while (1)
	 {
		 iov.iov_base = (void *)&frame;
		 iov.iov_len = sizeof(can_frame_t);
		 ret = rt_dev_recvmsg(rtcan0_socket, &msg, 0);

		 if(print_canrecv)
		 	rt_printf("putting message into the queue: \n");

		  if ((ret < 0)&&(print_error))
		{
		    switch (ret) 
		    {
			    case -ETIMEDOUT:
				    rt_printf("rt_dev_recv: timed out");
					break;
			    case -EBADF:
				    rt_printf("rt_dev_recv: aborted because socket was closed");
					break;
			    default:
					rt_printf("rt_dev_recv: %s\n", strerror(-ret));
				    break;
		    }
		}
		else
		{
			if(print_canrecv)
			{
				if(msgFlag == MSG_DONTROUTE)
					rt_printf("TX:  ");
				else
					rt_printf("RX:  ");


			    rt_printf("rtcan0 #%d: (%d) ", count, addr.can_ifindex);
				rt_printf("%lldns ", (long long)timestamp);
			    
			    if (frame.can_id & CAN_ERR_FLAG)
				rt_printf("!0x%08x!", frame.can_id & CAN_ERR_MASK);
			    else if (frame.can_id & CAN_EFF_FLAG)
				rt_printf("<0x%08x>", frame.can_id & CAN_EFF_MASK);
			    else
				rt_printf("<0x%03x>", frame.can_id & CAN_SFF_MASK);

			    rt_printf(" [%d]", frame.can_dlc);
			    if (!(frame.can_id & CAN_RTR_FLAG))
				for (i = 0; i < frame.can_dlc; i++) {
				    rt_printf(" %02x", frame.data[i]);
				}
			    if (frame.can_id & CAN_ERR_FLAG) {
				rt_printf(" ERROR ");
				if (frame.can_id & CAN_ERR_BUSOFF)
				    rt_printf("bus-off");
				if (frame.can_id & CAN_ERR_CRTL)
				    rt_printf("controller problem");
			    } else if (frame.can_id & CAN_RTR_FLAG)
				rt_printf(" remote request");
			    rt_printf("\n");
			}
			

		 
		 
		     /* send message */
		 	
		 	rxCan0Msg.count 		= count;
		 	rxCan0Msg.can_ifindex 	= addr.can_ifindex;
		 	rxCan0Msg.can_id 		= frame.can_id;
		 	rxCan0Msg.can_dlc 		= frame.can_dlc;
		 	rxCan0Msg.data[0] 		= frame.data[0];
		 	rxCan0Msg.data[1]		= frame.data[1];
		 	rxCan0Msg.data[2] 		= frame.data[2];
		 	rxCan0Msg.data[3] 		= frame.data[3];
		 	rxCan0Msg.data[4] 		= frame.data[4];
		 	rxCan0Msg.data[5] 		= frame.data[5];
		 	rxCan0Msg.data[6] 		= frame.data[6];
		 	rxCan0Msg.data[7] 		= frame.data[7];
		 	rxCan0Msg.timestamp 	= timestamp - startTime;
		 	rxCan0Msg.msgFlag 		= msgFlag;

		 	if(rxCan0Msg.can_id == 0x215)
		 	{
		 		/* Now, wait for a semaphore unit... */
        		rt_sem_p(&sem_desc,TM_INFINITE);
		 		 velocity = rxCan0Msg.data[0] | rxCan0Msg.data[0]<<8;
		 		/* then release it. */
        		rt_sem_v(&sem_desc);

		 		//velocity = 
		 		velocity = velocity * 0.0078125;
		 		rt_printf("data0: %d data1: %02x velocity = %lf",rxCan0Msg.data[0],rxCan0Msg.data[1],velocity);
		 		
		 	}
		 	rt_printf("size of logMsg = %d\n canID = %04x \n", sizeof(rxCan0Msg),rxCan0Msg.can_id);
		 	//rt_printf("put the message at this queue address %d \n",rtcan0BufferPrt);
			//retval = rt_queue_write(rtcan0BufferPrt,&rxCan0Msg,sizeof(rxCan0Msg),Q_NORMAL);

			if (retval < 0 ) {
				if(print_error)
			   		rt_printf("Sending error from rtcan0 = %d\n",retval);
			} else {
				if(print_canrecv)
			   		rt_printf("rtcan0 send message to recv buffer\n");
			}
			count++;

		}


		 
	 }


}
static void rtcan1MsgRecv_func(void *arg)
{
	int i, ret, count = 1;
    uint8_t msgFlag =0;
    struct can_frame frame;
    struct sockaddr_can addr;
    struct msghdr msg;
    struct iovec iov;
    canRxStruct rxCan1Msg;
	int retval = 0;
    

    nanosecs_abs_t timestamp = 0;

    msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = (void *)&addr;
	msg.msg_namelen = sizeof(struct sockaddr_can);
	msg.msg_control = (void *)&timestamp;
	msg.msg_controllen = sizeof(nanosecs_abs_t);
	msg.msg_flags = (void *)&msgFlag;
	 while (1)
	 {
		 iov.iov_base = (void *)&frame;
		 iov.iov_len = sizeof(can_frame_t);
		 ret = rt_dev_recvmsg(rtcan1_socket, &msg, 0);
		 if ((ret < 0)&&(print_error))
		{
		    switch (ret) 
		    {
			    case -ETIMEDOUT:
				    rt_printf("rt_dev_recv: timed out");
					break;
			    case -EBADF:
				    rt_printf("rt_dev_recv: aborted because socket was closed");
					break;
			    default:
					rt_printf("rt_dev_recv: %s\n", strerror(-ret));
					break;
		    }
		    
		}
		else 
		{
		
			if(print_canrecv)
			{
				 if(msgFlag == MSG_DONTROUTE)
					rt_printf("TX:  ");
				 else
					rt_printf("RX:  ");


			    rt_printf("rtcan1 #%d: (%d) ", count, addr.can_ifindex);
				rt_printf("%lldns ", (long long)timestamp);
			    
			    if (frame.can_id & CAN_ERR_FLAG)
				rt_printf("!0x%08x!", frame.can_id & CAN_ERR_MASK);
			    else if (frame.can_id & CAN_EFF_FLAG)
				rt_printf("<0x%08x>", frame.can_id & CAN_EFF_MASK);
			    else
				rt_printf("<0x%03x>", frame.can_id & CAN_SFF_MASK);

			    rt_printf(" [%d]", frame.can_dlc);
			    if (!(frame.can_id & CAN_RTR_FLAG))
				for (i = 0; i < frame.can_dlc; i++) {
				    rt_printf(" %02x", frame.data[i]);
				}
			    if (frame.can_id & CAN_ERR_FLAG) {
				rt_printf(" ERROR ");
				if (frame.can_id & CAN_ERR_BUSOFF)
				    rt_printf("bus-off");
				if (frame.can_id & CAN_ERR_CRTL)
				    rt_printf("controller problem");
			    } else if (frame.can_id & CAN_RTR_FLAG)
				rt_printf(" remote request");
			    rt_printf("\n");
			}	
		

		 
		 
		     /* send message */
		 	
		 	rxCan1Msg.count 		= count;
		 	rxCan1Msg.can_id 		= frame.can_id;
		 	rxCan1Msg.timestamp 	= timestamp-startTime;
		 	rxCan1Msg.msgFlag 		= msgFlag;
		 	rxCan1Msg.can_ifindex 	= addr.can_ifindex;
		 	rxCan1Msg.can_dlc 		= frame.can_dlc;
		 	rxCan1Msg.data[0] 		= frame.data[0];
		 	rxCan1Msg.data[1] 		= frame.data[1];
		 	rxCan1Msg.data[2] 		= frame.data[2];
		 	rxCan1Msg.data[3] 		= frame.data[3];
		 	rxCan1Msg.data[4] 		= frame.data[4];
		 	rxCan1Msg.data[5] 		= frame.data[5];
		 	rxCan1Msg.data[6] 		= frame.data[6];
		 	rxCan1Msg.data[7] 		= frame.data[7];

		 	if(print_canrecv)
		 	{
		 		rt_printf("size of logMsg = %d canID = %04x \n", sizeof(rxCan1Msg),rxCan1Msg.can_id);
		 	}
		 	if(rxCan1Msg.can_id == 0x215)
		 	{
		 		/* Now, wait for a semaphore unit... */
       			rt_sem_p(&sem_desc,TM_INFINITE);
		 		velocity = rxCan1Msg.data[0] | rxCan1Msg.data[0]<<8;
		 		/* then release it. */
        		rt_sem_v(&sem_desc);

		 		velocity = velocity * 0.0078125;
		 		rt_printf("data0: %d data1: %02x velocity = %lf",rxCan1Msg.data[0],rxCan1Msg.data[1],velocity);
		 		
		 	}

		 		
			//retval = rt_queue_write(rtcan1BufferPrt,&rxCan1Msg,sizeof(rxCan1Msg),Q_NORMAL);

			if (retval < 0 ) {
				if(print_error)
			   		rt_printf("Sending error from rtcan1 = %d\n",retval);
			} else {
				if(print_canrecv)
			   		rt_printf("taskOne sent message to mailbox\n");
			}
			count++;
		}

		 

	 }


}
int getVelocity()
{
	double ret=0;
	/* Now, wait for a semaphore unit... */
    rt_sem_p(&sem_desc,TM_INFINITE);
    ret = velocity;
    /* then release it. */
    rt_sem_v(&sem_desc);

    return ret;
}



static int canRecvStartOnItf(struct ifreq ifr, UINT8 itfName)
{

	int ret = CB_ERROR;
	struct sockaddr_can tempRecv_addr;
	static int temp_socket = -1;
	ret = rt_dev_socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (ret < 0) {
		if(print_canrecv)
		rt_printf(  "rt_dev_socket: %s\n", strerror(-ret));
	return CB_ERROR;
	}

	temp_socket = ret;

	
	ret = rt_dev_ioctl(temp_socket, SIOCGIFINDEX, &ifr);
	tempRecv_addr.can_family = AF_CAN;
	tempRecv_addr.can_ifindex = ifr.ifr_ifindex;
	//rt_printf("itfname:%s, itfIndex=%d\n", ifr.ifr_name, ifr.ifr_ifindex);
	ret = rt_dev_bind(temp_socket, (struct sockaddr *)&tempRecv_addr,
  	sizeof(struct sockaddr_can));
	if (ret < 0) {
		if(print_error)
        	rt_printf("rt_dev_bind: %s\n", strerror(-ret));
        goto failure;
    }
    
	ret = rt_dev_ioctl(temp_socket, RTCAN_RTIOC_TAKE_TIMESTAMP, RTCAN_TAKE_TIMESTAMPS);
    if (ret) 
    {
    	if(print_error)
        	rt_printf("rt_dev_ioctl TAKE_TIMESTAMP: %s\n", strerror(-ret));

        goto failure;
    }

    switch(itfName)
	{
		case ITF_RTCAN0:
			
			rtcan0_socket = temp_socket;
			rtcan0Recv_addr = tempRecv_addr;
			//rt_printf("start up rtcan0:%d\n",rtcan0_socket);
		    break;
	    case ITF_RTCAN1:
	    	
	    	rtcan1_socket = temp_socket;
			rtcan1Recv_addr = tempRecv_addr;
			//rt_printf("start up rtcan1:%d\n",rtcan1_socket);
		    break;		
	}
	return CB_SUCCESS;
    
	failure:
	    cleanup();
	    return CB_ERROR;  

}
enum responseCodes canfr_canrecvStop(void)
{
	return cleanup();
}
/**
 * @brief      timer that is used for timestamp received message will be reset to 0
 *
 * @return     - return 0 uppon success      
 *             - CB_ERROR, when it fail to start can channel(s)
 */
enum responseCodes canfr_canrecvResetClock(void)
{
	RT_TIMER_INFO infoTimer;
	rt_timer_inquire(&infoTimer);
	if(infoTimer.period == TM_ONESHOT)
	{
		startTime = (nanosecs_abs_t)rt_timer_read();
		return CB_SUCCESS;
	}
	else
	{
		if(print_error)
			rt_printf("the timer is not set to TM_ONESHOT");
		return CB_ERROR;
	}

}

/**
 * @brief      initilize and start up CAN receive function on interface describe on the input 
 *
 * @param[in]  telling which CAN channel to start recieving message base on busInterface value 
 *                      - Itf == ITF_RTCAN0    (==1) , start first channel 
 *                      - Itf == ITF_RTCAN1    (==2) , start second channel 
 *                      - Itf == ITF_RTCANALL  (==3) , start both channel
 *
 * @return     no meaning
 */
enum responseCodes canfr_canrecvStart(busInterface itfName)
{
	int  ret = CB_ERROR;
	struct ifreq ifr;
	itfName = itfName & 3;
	//nanosecs_abs_t timestamp = 0;
	int err;
	err = rt_sem_create(&sem_desc,"MySemaphore",SEM_INIT,SEM_MODE);

	switch(itfName)
	{
		case ITF_RTCANALL:
			strncpy(ifr.ifr_name, "rtcan0", IFNAMSIZ);
			ret = canRecvStartOnItf(ifr,ITF_RTCAN0);

			if (ret == CB_SUCCESS)
			{
				rt_task_create(&rtcan0MsgRecv_task, "rtcan0_Task", 0, 50, 0);
			    rt_task_start(&rtcan0MsgRecv_task, rtcan0MsgRecv_func, NULL);
			}
			else 
			{
				if (print_error)
					rt_printf("Can not start can recv at rtcan0\n");
			}
			
		    strncpy(ifr.ifr_name, "rtcan1", IFNAMSIZ);
		    ret = canRecvStartOnItf(ifr,ITF_RTCAN1);

		    if (ret == CB_SUCCESS)
		    {

		    	rt_task_create(&rtcan1MsgRecv_task, "rtcan1_Task", 0, 50, 0);
			    rt_task_start(&rtcan1MsgRecv_task, rtcan1MsgRecv_func, NULL);
		    }
		    else 
			{
				if (print_error)
					rt_printf("Can not start can recv at rtcan1\n");
			}
			break;
		case ITF_RTCAN0:

			strncpy(ifr.ifr_name, "rtcan0", IFNAMSIZ);
			ret = canRecvStartOnItf(ifr,ITF_RTCAN0);

			if (ret == CB_SUCCESS)
			{
				rt_task_create(&rtcan0MsgRecv_task, "rtcan0_Task", 0, 50, 0);
			    rt_task_start(&rtcan0MsgRecv_task, rtcan0MsgRecv_func, NULL);	
			}
			else 
			{
				if (print_error)
					rt_printf("Can not start can recv at rtcan0\n");
			}
	
			
		    break;
	    case ITF_RTCAN1:
	    	strncpy(ifr.ifr_name, "rtcan1", IFNAMSIZ);
			ret = canRecvStartOnItf(ifr,ITF_RTCAN1);

			if (ret == CB_SUCCESS)
			{
				rt_task_create(&rtcan0MsgRecv_task, "rtcan1_Task", 0, 50, 0);
			    rt_task_start(&rtcan0MsgRecv_task, rtcan0MsgRecv_func, NULL);	
			}
			else 
			{
				if (print_error)
					rt_printf("Can not start can recv at rtcan1\n");
			}

		    break;

		    default:
		    	if(print_error)
            		rt_printf("Unknown interface value ");
            ret =  CB_ERR_WRONG_BUS_TYPE;
            break;		
	}

	return ret;	
}

enum responseCodes canfr_canSetBuffer(busInterface itfName, RT_QUEUE* bufferPtr)
{
	int ret = CB_SUCCESS;


	 itfName = itfName & 3;
	 //rt_printf(" use buffer from this address: %d\n", bufferPtr);
    /* Get CAN interface name */
    switch(itfName)
    {
        case ITF_RTCANALL:
            if (print_canrecv)
            {
                rt_printf("the buffer is use for both can \n");   

            }
            	rtcan0BufferPrt = bufferPtr;
            	rtcan1BufferPrt = bufferPtr;
            break;
        case ITF_RTCAN0:
            if (print_canrecv)
            {
                rt_printf("the buffer is use for rtcan0 \n");      

            }
            	rtcan0BufferPrt = bufferPtr;
            break;
        case ITF_RTCAN1:
            if (print_canrecv)
            {
                rt_printf("the buffer is use for rtcan1 \n");     

            }
            	rtcan1BufferPrt = bufferPtr;
        default:
        	if(print_error)
            	rt_printf(  "Unknown interface value ");
            ret =  CB_ERR_WRONG_BUS_TYPE;
            break;

    }

	return ret;
}

/**
 * @}
 */
