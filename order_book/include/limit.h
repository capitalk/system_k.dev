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

#ifndef ORDER_BOOK_ORDER_BOOK_V2_LIMIT_H_
#define ORDER_BOOK_ORDER_BOOK_V2_LIMIT_H_

#include <time.h>
#include <stdint.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

#include <list>
#include <set>
#include <iostream>

#include "book_types.h"
#include "order.h"
#include "order_book.h"
#include "utils/types.h"

namespace capk {

class order_book;

class level {
  public:
    level(double price, uint32_t totalVolume = 0);

    virtual ~level();

    int addOrder(porder order);

    int removeOrder(uint32_t orderId);

    int modifyOrder(uint32_t orderId, double size);

    inline uint32_t getTotalVolume() const {
      return _totalVolume;
    }

    inline double getPrice() const {
      return _price;
    }

    inline uint32_t getOrderCount() const {
      return _orders.size();
    }

    inline timespec getUpdateTime() const {
      return _timeUpdated;
    }

    inline void setUpdateTime(const timespec updateTime) {
      _timeUpdated = updateTime;
    }

    inline timespec getMsgSentTime() const {
      return _msgSentTime;
    }

    inline void setMsgSentTime(const timespec sentTime) {
      _msgSentTime = sentTime;
    }

    bool operator<(const level& rhs) const {
      return (this->_price < rhs.getPrice());
    }

    bool operator>(const level& rhs) const {
      return (this->_price > rhs.getPrice());
    }

    bool operator()(const plevel& l1, const plevel& l2);

    friend std::ostream& operator<<(std::ostream& out, const capk::level& e);
    friend std::ostream& operator<<(std::ostream& out, const capk::order_book& b);

  private:
    Orders _orders;
    double _price;
    uint32_t _totalVolume;
    timespec _timeUpdated;
    timespec _msgSentTime;
    inline uint32_t _addVolume(uint32_t vol) {
    _totalVolume += vol;
#ifdef DEBUG
      std::cerr << "TOTAL VOLUME(@"
          << _price
          << ") NOW (+ "
          << vol
          << "):"
          << _totalVolume
          << std::endl;
#endif
      return _totalVolume;
    }

    inline uint32_t _removeVolume(uint32_t vol) {
      _totalVolume -= vol;
#ifdef DEBUG
      std::cerr << "TOTAL VOLUME(@"
          << _price
          << ") NOW (- "
          << vol
          << "):"
          << _totalVolume
          << std::endl;
#endif
      return _totalVolume;
    }

    Orders::iterator _findOrder(uint32_t orderId);
    int _removeOrder(uint32_t orderId);
};

}  // namespace capk
#endif  // ORDER_BOOK_ORDER_BOOK_V2_LIMIT_H_

