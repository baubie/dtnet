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

vector<double> Results::meanSpikeCount(const string popID, const string ID) {
    // Return the mean spike counts over all simulations.
    // When the result is constrained to have zero parameters, this is
    // equivalent to mean spike count over all trials.
    vector<double> means;
    if (this->unconstrained.find(ID) == this->unconstrained.end()) {
        cout << "[X] Mean spike count requested on a parameter that is not unconstrained." << endl;
        return means; 
    }

    for (Range::iterator sIter = this->unconstrained[ID].begin(); sIter != this->unconstrained[ID].end(); ++sIter) {
        int total = 0;
        int count = 0;
        vector<Results::Result*> r = this->get(ID, *sIter);
        for (vector<Results::Result*>::iterator result = r.begin(); result != r.end(); ++result) {
            for (vector<Neuron>::iterator neuron = (*result)->cNetwork.populations[popID].neurons.begin();
                                          neuron != (*result)->cNetwork.populations[popID].neurons.end();
                                          ++neuron) {
                total += neuron->spikes.size();
                ++count; 
            }
        }
        means.push_back((double)total/(double)count);
    }

    return means;
}

void Results::add(Result &r) {
    results[results_size] = r; // Add it to the pile.
    this->filter.push_back(results_size); // Add on the last index
    ++results_size;
}

void Results::init(int size) {
    results_size = 0;
    results.clear();
    results.resize(size);
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
    if (type_ID == "connection") {
        string from, to;
        pos = item_ID.find(':');
        from = item_ID.substr(0, pos);
        to = item_ID.substr(pos+1);
        if (param_ID == "weight" && r.cNetwork.connections[to][from].weight == value) matches = true;
        if (param_ID == "delay" && r.cNetwork.connections[to][from].delay == value) matches = true;
        if (param_ID == "density" && r.cNetwork.connections[to][from].density == value) matches = true;
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
    r.filter.clear(); // Make sure we clear the filter and start fresh.

    // Loop over all the results in this collection.
    // Add them onto the new one only if the constraint matches
    for (vector<int>::iterator i = this->filter.begin(); i != this->filter.end(); ++i) {
        if (this->matches(results[*i], ID, value)) { r.filter.push_back(*i); }
    }
    return true;
}

void Results::save(string filename) {
    ofstream ofs(filename.c_str());

#ifdef SERIALIZE_TEXT
    LOG("Saving Text Archive to " << filename.c_str());
    boost::archive::text_oarchive oa(ofs);
#else
    LOG("Saving Binary Archive to " << filename.c_str());
    boost::archive::binary_oarchive oa(ofs);
#endif
    LOG("boost::archive created.");
    oa << *this;    
}

bool Results::load(Results &r, string filename) {
    ifstream ifs(filename.c_str());
    if (ifs.fail()) {
        cout << "[X] Error opening " << filename << ".  Sorry." << endl;
        return false;
    }
#ifdef SERIALIZE_TEXT
    boost::archive::text_iarchive ia(ifs);
#else
    boost::archive::binary_iarchive ia(ifs);
#endif
    ia >> r;    
    return true;
}
