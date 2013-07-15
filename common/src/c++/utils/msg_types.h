//
// System K - A framework for building trading applications and analysis
// Copyright (C) 2013 Timir Karia <tkaria@capitalkpartners.com>
//
// This file is part of System K.
//
// System K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// System K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with System K.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __KMSG_TYPES_H__
#define __KMSG_TYPES_H__

#include "utils/types.h"

// Types for messages between client and order proxy
// These are used as message headers for ZMQ messages - 
// sent as the first part of the envelope
namespace capk {

// Incoming messages 
const msg_t ORDER_NEW		      = 0x01;
const msg_t ORDER_CANCEL	    = 0x02;
const msg_t ORDER_REPLACE	    = 0x03;
const msg_t ORDER_STATUS	    = 0x04;

const msg_t ORDER_ACK		      = 0x05;
const msg_t ORDER_CANCEL_REJ  = 0x06;

const msg_t LIST_STATUS		    = 0x07;

const msg_t STRATEGY_HELO	    = 0xF0;
const msg_t STRATEGY_HELO_ACK = 0xF1;


const msg_t HEARTBEAT		      = 0xA0;
const msg_t HEARTBEAT_ACK	    = 0xA1;

const msg_t EXEC_RPT		      = 0xB0;

const msg_t POSITION_REQ      = 0xC0;
const msg_t OPEN_ORDER_REQ    = 0xC1;

const msg_t PING_REQ          = 0xF2;
const msg_t PING_ACK          = 0xF3;

}; // namespace capk

#endif // __KMSG_TYPES_H__
