#ifndef LOGGING_H_566e1626_2717_477d_ad08_eb360acace8b_
#define LOGGING_H_566e1626_2717_477d_ad08_eb360acace8b_

// Primary header file, always be included
#include "pantheios/pantheios.hpp" 

// Inserters for ridiculous type safety mechanism in Pantheios
#include "pantheios/inserters/boolean.hpp"
#include "pantheios/inserters/character.hpp"
#include "pantheios/inserters/args.hpp"
#include "pantheios/inserters/integer.hpp"
#include "pantheios/inserters/real.hpp"
#include "pantheios/inserters/blob.hpp"
#include "pantheios/inserters/pointer.hpp"

// Simple front end - print all levels 
#include "pantheios/frontends/fe.simple.h"

// Multiplexing backend and concrete backends
#include "pantheios/backends/be.N.h"
#include "pantheios/backends/bec.fprintf.h"
#include "pantheios/backends/bec.file.h"

namespace pan = pantheios;


//Specify process identity
extern const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[];

//Use pantheios::log_xxx() or pantheios::log(xxx, ) with xxx is severity level
int logging_init(const char*); 

#endif //  LOGGING_H_566e1626_2717_477d_ad08_eb360acace8b_
