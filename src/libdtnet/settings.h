#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <string>

/** Settings class implemented with the singleton pattern. **/
class Settings {
public:
    std::string get_str(std::string const var);
    double get_dbl(std::string const var);
    void set(std::string const var, double const val);
    void set(std::string const var, std::string const val);

    static Settings *s_instance;

    static Settings *instance() {
        if (!s_instance)
            s_instance = new Settings;
        return s_instance;
    }

private:

    std::map<std::string, std::string> values_string;
    std::map<std::string, double> values_double;
};



#endif
