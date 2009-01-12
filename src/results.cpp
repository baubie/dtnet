
#include "results.h"

using namespace std;

// Empty constructor that is only used for passing a results object to the simulation class.
Results::Results() { }
Results::Results(double T, double dt, double delay) : T(T), dt(dt), delay(delay) {}

string Results::toString() {
    stringstream r;
    r << "Result Collection" << endl;
    r << "=================" << endl;
    r << this->results.size() << " results found." << endl;
    r << this->unconstrained.size() << " unconstrained variables." << endl;
    for (map<string, Range>::iterator i = this->unconstrained.begin(); i != this->unconstrained.end(); ++i) {
        r << " - " << i->first << " " << i->second.toString() << endl;
    }
    return r.str(); 
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
