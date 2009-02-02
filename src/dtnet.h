
#ifndef DTNET_H
#define DTNET_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <boost/program_options.hpp>
#include "GLE.h"
#include "net.h"
#include "trial.h"
#include "vt100.h"
#include "lib/threadpool/threadpool.hpp"
#include "dtlang.h"
#include "lib/SReadline/SReadline.h"

extern std::string GLE::viewer;
extern bool GLE::qgle;
extern bool dtlang::verbose;

#endif
