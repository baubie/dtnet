
#ifndef PARSEARGS_H
#define PARSEARGS_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>

int parseargs(int argc, char* argv[], bool& verbose, std::string& script, int& procs, std::string& viewer);
void outputHelp();

#endif

