
#include "range.h"

double Range::operator=(const Range& obj) {
    return this->start;
}

Range Range::operator=(const double& val) {
    return Range(val);
}
