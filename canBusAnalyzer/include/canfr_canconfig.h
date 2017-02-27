#ifndef CANFRMSG_CANCONFIG_H
#define CANFRMSG_CAN_H

#include <canfrMsg.h>
#include <canfr_typedef.h>

int canfr_canActivate(busInterface itfName, UINT8 flags);
int canfr_canDeactivate(busInterface itfName, UINT8 flags);
int canfr_canSetBit(busInterface itfName, UINT32 baudRate);
static int canSetModeOnItf(struct ifreq ifr, int newMode);
static int canSetbitRateOnItf(struct  ifreq ifr, UINT32 new_baudRate);
// int canSetbitRateOnItf(struct  ifreq ifr, UINT32 new_baudRate);




#endif
