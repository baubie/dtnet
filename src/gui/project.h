#ifndef PROJECT_H
#define PROJECT_H

#include <string>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>

class Project
{
    public:
        std::string networkFilename;
        std::string trialFilename;

    private:

};


#endif
