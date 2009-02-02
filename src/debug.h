#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define LOG(msg) std::clog << "[LOG] " << msg << std::endl;
#else
#define LOG(msg)
#endif


#endif
