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

#ifndef COLLECT_FIX_BROADCAST_H_
#define COLLECT_FIX_BROADCAST_H_

#include "utils/types.h"

namespace capk {

const int MAX_MSG_SIZE = 256;

void broadcast_bbo_book(void* bcast_socket,
                        const char* symbol,
                        const double best_bid,
                        const double best_ask,
                        const double bbsize,
                        const double basize,
                        const capk::venue_id_t venue_id);

} // namespace capk
#endif // COLLECT_FIX_BROADCAST_H_


