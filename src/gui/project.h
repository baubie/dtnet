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
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & networkFilename;
            ar & trialFilename;
        }
};


#endif
