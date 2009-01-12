
#include "results.h"

using namespace std;

// Empty constructor that is only used for passing a results object to the simulation class.
Results::Results() : use_external(false) { }
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

vector< Results::Result* > Results::get() {
    vector< Result* > r;
    if (use_external == false) {
        // If we are local, then we haven't gone through constrain().
        // So we can assume that we want the entire vector.
        for (vector<Result>::iterator i = this->results.begin(); i != this->results.end(); ++i) {
            r.push_back(&(*i));
        }
    } else {
        // Since we are external, we have gone through constrain().
        // Therefore, we must use the filter.
        for (vector<int>::iterator i = this->filter.begin(); i != this->filter.end(); ++i) {
            r.push_back(&(this->external_results->at(*i)));
        }
    }
    return r;
}
void Results::add(Result &r) {
    this->results.push_back(r); // Add it to the pile.
    this->filter.push_back(this->results.size() - 1); // Add on the last index
}

void Results::setExternal(std::vector< Result> *ext) {
    this->external_results = ext;
}

Results Results::constrain(std::string ID, const double value) {

    // If the key isn't unconstrained, just return what we have and print a warning.
    if (this->unconstrained.find(ID) == this->unconstrained.end()) {
        cout << "[WARNING] " << ID << " is not constrained.  Original results collection returned." << endl;
        return *this;
    }

    Results r(this->T, this->dt, this->delay);
    r.use_external = true;

    if (this->use_external = false) {
        r.setExternal(&(this->results)); // Point the new results to these results.
    } else {
        r.setExternal(this->external_results); // Point the new results to the same as this one.
    }
    r.unconstrained = this->unconstrained;
    r.unconstrained.erase(ID); // Remove it from the list;

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


    // Loop over all the results in this collection.
    // Add them onto the new one only if the constraint matches
    vector<Result*> old = this->get();
    for (vector<Result*>::iterator i = old.begin(); i != old.end(); ++i) {
        bool add = false;
        if (type_ID == "trial") {
            for (vector<Input::Signal>::iterator input = i->cTrial.signals.begin(); input != i->cTrial.signals.end(); ++input) {

            }
        } 

        if (add) {
            r.filter.push_
        }
    }

    return r;
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
