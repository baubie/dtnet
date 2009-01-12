
#ifndef RANGE_H
#define RANGE_H

#include "serialization.h"
#include <vector>
#include <string>
#include <sstream>

class Range {

    public:
        Range();
        Range(double val);
        Range(double start, double end, double step);
        int size();

        Range operator=(const double& val);
        operator const double() { return this->values.at(0); }
        std::string toString();


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