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

#ifndef ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_
#define ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_

#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <list>

namespace capk {
    class level;
    class order;
    class order_book;
    class level_comparator;
    typedef boost::shared_ptr<level> plevel;
    typedef boost::shared_ptr<order> porder;
    typedef std::list<porder> Orders;
    typedef std::set<plevel, level_comparator> limit_tree;
    typedef std::map<uint32_t, porder> order_map;
}

#endif  // ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_
