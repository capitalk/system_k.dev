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

#ifndef CAPK_CONSTANTS
#define CAPK_CONSTANTS

#define MIC_LEN 9
#define SYMBOL_LEN 8
#define CURRENCY_SYMBOL_LEN 4
#define MAX_MICS 126
#define MAX_VENUES 126

#include <limits.h>

namespace capk {

const int32_t INIT_BID  = INT_MIN;
const int32_t INIT_ASK  = INT_MAX;
const int32_t NO_BID = INIT_BID;
const int32_t NO_ASK = INIT_ASK;
const double INIT_SIZE = -1;
const double NO_SIZE = 0;

const uint32_t ZMQ_MAX_MSG_HOPS = 4;
const uint32_t ZMQ_ADDR_LEN = 17;
const uint32_t UUID_LEN = 16;
const uint32_t UUID_STRLEN  = 36;
}


#endif // CAPK_CONSTANTS
