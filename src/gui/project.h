#ifndef PROJECT_H
#define PROJECT_H

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>

class Project
{
    public:
        std::list<std::string> networkFilename;
        std::list<std::string> trialFilename;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & networkFilename;
            ar & trialFilename;
        }
};


#endif
