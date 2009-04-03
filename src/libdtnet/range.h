
#ifndef RANGE_H
#define RANGE_H

#include "serialization.h"
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include "debug.h"

class Range {

    public:
        Range();
        Range(double val);
        Range(double start, double end);
        Range(double start, double end, double step);
        void merge(Range &r);

        Range operator=(const double& val);
        operator const double() { return this->values.at(0); }
        std::string toString();

        // Pretend to be a collection
        typedef std::vector<double>::iterator iterator;
        std::vector<double>::iterator begin();
        std::vector<double>::iterator end();
        double front();
        double back();
        double max();
        double min();
        int size();

        std::vector<double> values;

    private:

        void initialize(double start, double end, double step);

#ifdef BUILDING_LIBRARY
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & values;
        }
#endif

};

#endif
