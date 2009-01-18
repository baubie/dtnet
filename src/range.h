
#ifndef RANGE_H
#define RANGE_H

#include "serialization.h"
#include <vector>
#include <string>
#include <sstream>
#include "debug.h"

class Range {

    public:
        Range();
        Range(double val);
        Range(double start, double end, double step);
        int size();

        Range operator=(const double& val);
        operator const double() { return this->values.at(0); }
        std::string toString();

        // Pretend to be a collection
        typedef std::vector<double>::iterator iterator;
        std::vector<double>::iterator begin();
        std::vector<double>::iterator end();
        double front();
        double back();


        std::vector<double> values;

    private:

        void initialize(double start, double end, double step);

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & values;
        }
};

#endif
