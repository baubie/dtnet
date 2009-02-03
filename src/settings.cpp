
#include "settings.h"

std::string Settings::get_str(std::string const var) {
    if (this->values_string.find(var) != this->values_string.end()) {
        return this->values_string[var];
    }
    return "";
}

double Settings::get_dbl(std::string const var) {
    if (this->values_double.find(var) != this->values_double.end()) {
        return this->values_double[var];
    }
    return 0;
}

void Settings::set(std::string const var, std::string const val) {
    this->values_string[var] = val;
}

void Settings::set(std::string const var, double const val) {
    this->values_double[var] = val;
}
