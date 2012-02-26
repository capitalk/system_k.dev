#ifndef __KMSG_TYPES_H__
#define __KMSG_TYPES_H__

#define ORDER_NEW 0x01
#define ORDER_CAN 0x03
#define ORDER_REP 0x05
#define ORDER_REJ 0x07
#define ORDER_FIL 0x09
#define ORDER_STA 0x0B

#define ORDER_NEW_ACK (ORDER_NEW + 1)
#define ORDER_CAN_ACK (ORDER_CAN + 1) 
#define ORDER_REP_ACK (ORDER_REP + 1)
#define ORDER_REJ_ACK (ORDER_REJ + 1)
#define ORDER_FIL_ACK (ORDER_FIL + 1)
#define ORDER_STA_ACK (ORDER_STA + 1)

#endif // __KMSG_TYPES_H__
