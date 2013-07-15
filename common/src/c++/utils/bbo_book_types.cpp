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

#include "bbo_book_types.h"
#include "venue_globals.h"

namespace capk {

void
InstrumentBBO_init(InstrumentBBO_t* i) 
{
    assert(i);
    if (!i) {
        return;
    }
    memset(i->symbol, 0, SYMBOL_LEN);
    
    i->bid_venue_id = NULL_VENUE_ID;
    i->bid_price = NO_BID;
    i->bid_size = INIT_SIZE;
    memset(&(i->bid_last_update), 0, sizeof(timespec));

    i->ask_venue_id = NULL_VENUE_ID;
    i->ask_price = NO_ASK;
    i->ask_size = INIT_SIZE;
    memset(&(i->ask_last_update), 0, sizeof(timespec));
}

} // namespace capk

