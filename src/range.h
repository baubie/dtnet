
#ifndef RANGE_H
#define RANGE_H

#include "serialization.h"

class Range {
    Range() : start(0),end(0),step(1) {}
    Range(double val) : start(val),end(val),step(1) {}
    double start;
    double end;
    double step;

    double operator=(const Range& obj);
    Range operator=(const double& val);

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & start;
        ar & end;
        ar & step;
    }
};

#endif
