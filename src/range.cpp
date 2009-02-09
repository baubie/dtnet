
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

Range::Range(double start, double end) {
    this->initialize(start, end, 1);
}

void Range::initialize(double start, double end, double step) {
    this->values.clear();
    if (end >= start) {
        for(double i = start; i <= end; i += step) {
            this->values.push_back(i);
        }
    } else {
        for(double i = start; i >= end; i += step) {
            this->values.push_back(i);
        }
    }
}

void Range::merge(Range &r) {
    for(vector<double>::iterator i = r.values.begin(); i != r.values.end(); ++i) {
        this->values.push_back(*i);
    }
    sort(this->values.begin(), this->values.end());
    vector<double>::iterator it = unique(this->values.begin(), this->values.end());
    this->values.resize(it - this->values.begin());
}

vector<double>::iterator Range::begin() { return this->values.begin(); }
vector<double>::iterator Range::end() { return this->values.end(); }
double Range::front() { return this->values.front(); }
double Range::back() { return this->values.back(); }
double Range::max() { return *max_element(this->values.begin(), this->values.end()); }
double Range::min() { return *min_element(this->values.begin(), this->values.end()); }

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
