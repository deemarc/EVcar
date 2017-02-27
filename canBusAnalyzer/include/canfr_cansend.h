#ifndef CANFRMSG_CANRECV_H
#define CANFRMSG_CANRECV_H

#include <canfrMsg.h>
#include <canfr_typedef.h>
//#include<rtcanrecv.h>
//#include<rtcansend.h>

/*
 * can_init: its task are;
 * run rtcanconfig to get start rtcan interface
 * run rtcanrecv init rt_task for it
 * run rtcansend init rt_task for it
 * if all success notify the client with 0
 * not success notifu the clent with -1
 */
int canfr_cansendInit(void);
int canfr_cansendStop(void);
int canfr_canTransmit(canSend_struc inCanMsg);


#endif