#ifndef __KMSG_TYPES_H__
#define __KMSG_TYPES_H__

// Types for messages between client and order proxy
// These are used as message headers for ZMQ messages - 
// sent as the first part of the envelope
namespace capk {

typedef unsigned int msg_t;
typedef int venue_id_t;

// Incoming messages 
const msg_t ORDER_NEW		= 0x01;
const msg_t ORDER_CANCEL	= 0x02;
const msg_t ORDER_REPLACE	= 0x03;
const msg_t ORDER_STATUS	= 0x04;

const msg_t ORDER_ACK		= 0x05;
const msg_t ORDER_CANCEL_REJ= 0x06;

const msg_t LIST_STATUS		= 0x07;

const msg_t STRATEGY_HELO	= 0xF0;
const msg_t STRATEGY_HELO_ACK = 0xF1;

const msg_t HEARTBEAT		= 0xA0;
const msg_t HEARTBEAT_ACK	= 0xA1;

const msg_t EXEC_RPT		= 0xB0;



}; // namespace capk

#endif // __KMSG_TYPES_H__
