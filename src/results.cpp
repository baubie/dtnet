
#include "results.h"

using namespace std;

extern vector< Results::Result > Results::results;

// Empty constructor that is only used for passing a results object to the simulation class.
Results::Results() { }
Results::Results(double T, double dt, double delay) : T(T), dt(dt), delay(delay) {}

string Results::toString() {
    stringstream r;
    r << "Result Collection" << endl;
    r << "=================" << endl;
    r << this->get().size() << " results found." << endl;
    r << "=================" << endl;
    r << this->unconstrained.size() << " unconstrained variables." << endl;
    for (map<string, Range>::iterator i = this->unconstrained.begin(); i != this->unconstrained.end(); ++i) {
        r << " - " << i->first << " " << i->second.toString() << endl;
    }
    return r.str(); 
}

vector< Results::Result* > Results::get() {
    return this->get(string(""), 0);
}

vector< Results::Result* > Results::get(const string ID, const double value) {
    vector< Result* > r;
    vector<Result>::pointer ptr;
    for (vector<int>::iterator i = this->filter.begin(); i != this->filter.end(); ++i) {
        ptr = &(results[*i]);
        if (ID == "" || this->matches(*ptr, ID, value)) {
            r.push_back(ptr);
        }
    }
    return r;
}

void Results::add(Result &r) {
    results.push_back(r); // Add it to the pile.
    this->filter.push_back(results.size() - 1); // Add on the last index
}

bool Results::matches(Result &r, string ID, const double value) {

    // Split up the ID into its three parts.
    string type_ID;
    string item_ID;
    string param_ID;
    size_t pos;
    pos = ID.find('.');
    type_ID = ID.substr(0, pos);
    ID = ID.substr(pos+1);
    pos = ID.find('.');
    item_ID = ID.substr(0, pos);
    param_ID = ID.substr(pos+1);

    bool matches = false;
    if (type_ID == "trial") {
        for (vector<Input::Signal>::iterator input = r.cTrial.signals.begin(); input != r.cTrial.signals.end(); ++input) {
            if (input->ID == item_ID) {
                if (param_ID == "duration" && input->duration == value) matches = true;
                if (param_ID == "amplitude" && input->amplitude == value) matches = true;
                if (param_ID == "delay" && input->delay == value) matches = true;
            } 
        }
    } 

    return matches;
}

bool Results::constrain(Results &r, std::string ID, const double value) {

    // If the key isn't unconstrained, just return what we have and print a warning.
    if (this->unconstrained.find(ID) == this->unconstrained.end()) {
        cout << "[WARNING] " << ID << " is not constrained.  Original results collection returned." << endl;
        return false;
    }

    r.T = this->T;
    r.dt = this->dt;
    r.delay = this->delay;
    r.unconstrained = this->unconstrained;
    r.timeseries = this->timeseries;
    r.unconstrained.erase(ID); // Remove it from the list;

    // Loop over all the results in this collection.
    // Add them onto the new one only if the constraint matches
    for (vector<int>::iterator i = this->filter.begin(); i != this->filter.end(); ++i) {
        if (this->matches(results[*i], ID, value)) { r.filter.push_back(*i); }
    }
    return true;
}

void Results::save(string filename) {
    ofstream ofs(filename.c_str());
    boost::archive::binary_oarchive oa(ofs);
    oa << *this;    
}

bool Results::load(Results &r, string filename) {
    ifstream ifs(filename.c_str());
    if (ifs.fail()) {
        cout << "[X] Error opening " << filename << ".  Sorry." << endl;
        return false;
    }
    boost::archive::binary_iarchive ia(ifs);
    ia >> r;    
    return true;
}
