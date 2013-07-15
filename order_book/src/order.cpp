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

#include "order.h"

namespace capk {

std::ostream& operator<<(std::ostream& out, const capk::order& e) {
  out << "ORDER: "
      << ":"
      << e._orderId
      << ":"
      << e._buySell
      << ":"
      << e._size
      << ":"
      << e._price;
  return out;
}

order::order(uint32_t orderId,
               capk::Side_t buySell,
               double size,
               double price) {
  _orderId = orderId;
  _buySell = buySell;
  _size = size;
  _price = price;
}

order:: ~order() {
}

}  // namespace capk

