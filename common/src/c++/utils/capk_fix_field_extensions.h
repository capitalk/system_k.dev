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

#ifndef _CAPK_FIXFIELDS_
#define _CAPK_FIXFIELDS_

#include "quickfix/Field.h"

namespace FIX
{
    USER_DEFINE_STRING(K_ReplayFile, 2000);
    USER_DEFINE_FLOAT(K_Volatility, 2001);
    USER_DEFINE_FLOAT(K_ReplayTimeDiv, 2002);
}

#endif // _CAPK_FIXCONVERTORS_
