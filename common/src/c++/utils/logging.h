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

#ifndef LOGGING_H_566e1626_2717_477d_ad08_eb360acace8b_
#define LOGGING_H_566e1626_2717_477d_ad08_eb360acace8b_

// Primary header file, always be included
#include "pantheios/pantheios.hpp" 

// Inserters for type safety mechanism in Pantheios
#include "pantheios/inserters/boolean.hpp"
#include "pantheios/inserters/character.hpp"
#include "pantheios/inserters/args.hpp"
#include "pantheios/inserters/integer.hpp"
#include "pantheios/inserters/real.hpp"
#include "pantheios/inserters/blob.hpp"
#include "pantheios/inserters/pointer.hpp"

// Simple front end - print all levels 
//#include "pantheios/frontends/fe.simple.h"
#include "pantheios/frontends/fe.N.h"

// Multiplexing backend and concrete backends
#include "pantheios/backends/be.N.h"
#include "pantheios/backends/bec.fprintf.h"
#include "pantheios/backends/bec.file.h"

namespace pan = pantheios;


//Specify process identity
//extern const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[];

//Use pantheios::log_xxx() or pantheios::log(xxx, ) with xxx is severity level
int logging_init(const char*); 

std::string createTimestampedLogFilename(const char* prefix);
#endif //  LOGGING_H_566e1626_2717_477d_ad08_eb360acace8b_
