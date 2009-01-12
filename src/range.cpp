
#include "range.h"

using namespace std;

Range::Range() {
    this->initialize(0,0,1);
}

Range::Range(double val) {
    this->initialize(val,val,1);
}

Range::Range(double start, double end, double step) {
    this->initialize(start, end, step);
}

void Range::initialize(double start, double end, double step) {
    this->values.clear();
    for(double i = start; i <= end; i += step) {
        this->values.push_back(i);
    }
}

int Range::size() {
    return this->values.size();
}

Range Range::operator=(const double& val) {
    return Range(val);
}

string Range::toString() {
    stringstream s;
    s << "[";
    for (vector<double>::iterator i = this->values.begin(); i != this->values.end(); ++i) {
        s << *i;
        if (i != this->values.end()-1) s << ",";
    }
    s << "]";
    return s.str();
}
