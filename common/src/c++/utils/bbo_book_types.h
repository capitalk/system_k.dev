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

#ifndef CAPK_BBO
#define CAPK_BBO

#include <time.h>
#include "types.h"
#include "constants.h"


namespace capk {

struct IBSingleMarketBBO {
	char symbol[SYMBOL_LEN];
  char currency[CURRENCY_SYMBOL_LEN];
  double bid_price;
  double ask_price;
  double bid_size;
  double ask_size;
  timespec last_update; 
  double last_price;
  double last_size; 
};
typedef struct IBSingleMarketBBO IBSingleMarketBBO_t;

struct SingleMarketBBO {
  venue_id_t venue_id;
	char symbol[SYMBOL_LEN];
  double bid_price;
  double ask_price;
  double bid_size;
  double ask_size;
  timespec last_update; 
    
};
typedef struct SingleMarketBBO SingleMarketBBO_t;


struct InstrumentBBO {
  char symbol[SYMBOL_LEN];
  venue_id_t bid_venue_id;
  double bid_price;
  double bid_size;
  timespec bid_last_update;
  capk::venue_id_t ask_venue_id;
  double ask_price;
  double ask_size;
  timespec ask_last_update;
};

typedef struct InstrumentBBO InstrumentBBO_t;

void InstrumentBBO_init(InstrumentBBO_t* i);

} // namespace capk

#endif // CAPK_BBO
