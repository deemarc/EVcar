/**
 * 
 * @addtogroup canrecv
 * @{
 * @file canfr_canrecv.h
 */

#ifndef CANFRMSG_CANRECV_H
#define CANFRMSG_CANRECV_H

#include <canfrMsg.h>
#include <canfr_typedef.h>
//#include<rtcanrecv.h>
//#include<rtcansend.h>


/**
 * @anchor canRxStruct
 * @brief a structure for storing all received CAN message, 
 * 
 * this structure is used for storing all recieved CAN message and it is also the same structure being sent to CANNIBESSA when being requested.
 * - it's struct size is 32 bytes
 */
 typedef struct 
{ 
  unsigned long count;///<count up recieived CAN message after start receiving
  unsigned long can_id;
                       ///<to know which CAN message came fisrt and what how much is revecived until now
  unsigned long long timestamp;///<CAN message's time of arrival
  /**
   * CAN message's flag indicate whether it is a transimitted(TX) message or received(RX) message
   * - 0 For TX
   * - 1 for RX
   */
  unsigned char msgFlag; 
  unsigned char can_ifindex;///<interface value telling whether the CAN message coming from channel 1 or 2
  unsigned char can_dlc; ///<CAN message's DLC value telling how long the CAN message is 0 - 8
  unsigned char reserveByte[5]; ///<reserved byte to meet with data structure's constrain                        
  unsigned char data[8];/// __attribute__ ((aligned(8)))
                        ///<CAN message's data
} canRxStruct;


enum responseCodes canfr_canrecvStart(busInterface itfName);
enum responseCodes canfr_canrecvResetClock(void);
enum responseCodes canfr_canrecvStop(void);
int getVelocity(void);
enum responseCodes canfr_canSetBuffer(busInterface itfName, RT_QUEUE* bufferPtr);



#endif

/**
 * @}
 */
