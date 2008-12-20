
#ifndef PARSEARGS_H
#define PARSEARGS_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>

using namespace std;

int parseargs(int argc, char* argv[], bool& verbose, string& script, int& procs);
void outputHelp();


#endif

