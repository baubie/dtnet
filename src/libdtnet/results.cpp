#include "results.h"

using namespace std;

// Empty constructor that is only used for passing a results object to the simulation class.
Results::Results() {}
Results::Results(double T, double dt, double delay) : T(T), dt(dt), delay(delay) {}

double stddev(vector<double> &vals) {
    if (vals.empty()) return 0;

    double mean = 0;
    double stddev = 0;
    vector<double>::iterator i;
    for (i = vals.begin(); i != vals.end(); ++i) {
        mean += *i;
    }
    mean /= vals.size();
    for (i = vals.begin(); i != vals.end(); ++i) {
        stddev += pow(*i-mean,2);
    }
    stddev /= vals.size();
    stddev = sqrt(stddev);
    return stddev;
}
double stderror(vector<double> &vals) {
    if (vals.size() == 0) return 0;
    return stddev(vals) / sqrt(vals.size());
}


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

// Return a vector of pointers to the results
vector< Results::Result* > Results::get(const string ID, const double value) {
    vector< Result* > r;
    vector<Result>::pointer ptr;
    for (vector<Result*>::iterator i = this->filter.begin(); i != this->filter.end(); ++i) {
        if (ID == "" || this->matches(**i, ID, value)) {
            r.push_back(*i);
        }
    }
    return r;
}

boost::tuple< vector<double>, vector<double>, vector<double> > Results::firstSpikeLatency(const string popID, const string ID) {
    vector<double> latency;
    vector<double> err_up;
    vector<double> err_down;

    if (this->unconstrained.find(ID) == this->unconstrained.end()) {
        cout << "[X] Mean spike count requested on a parameter that is not unconstrained." << endl;
        return latency; 
    }

    for (Range::iterator sIter = this->unconstrained[ID].begin(); sIter != this->unconstrained[ID].end(); ++sIter)
    {
        int count = 0;
        double cur = 0;
        double total = 0;
        vector<Results::Result*> r = this->get(ID, *sIter);
        vector<double>::iterator spike;
        vector<double> times;
        times.clear();
        for (vector<Results::Result*>::iterator result = r.begin(); result != r.end(); ++result) 
        {
            for (list<Neuron*>::iterator neuron = (*result)->cNetwork.populations[popID].neurons.begin();
                                          neuron != (*result)->cNetwork.populations[popID].neurons.end();
                                          ++neuron) 
            {
                spike = (*neuron)->spikes.begin();
                while (spike != (*neuron)->spikes.end() && *spike < 0) { ++spike; }
                
                // We found a spike beyond time 0
                if (spike != (*neuron)->spikes.end()) { total += *spike; ++count; times.push_back(*spike); }
            }
        }
        if (count == 0) count = 1;
        latency.push_back((double)total/(double)count);
        double se = stderror(times);
        err_up.push_back(se);
        err_down.push_back(se);
    }

    return boost::tuple<vector<double>, vector<double>, vector<double> > (latency, err_up, err_down);
}

boost::tuple< vector<double>, vector<double> > Results::psth(const std::string popID, double window_size) {

    vector<double> c;
    vector<double> x;

    int count = 0;
    double window = this->timeseries.front();
    vector<Results::Result*> r = this->get();
    for (vector<double>::iterator i = this->timeseries.begin(); i != this->timeseries.end(); ++i) {
        if (*i - window >= window_size) {
           x.push_back(window + window_size / 2);
           c.push_back(count);
           window = *i;
           count = 0; 
        }
        // Loop over the spikes
        for (vector<Results::Result*>::iterator result = r.begin(); result != r.end(); ++result) {
            for (list<Neuron*>::iterator neuron = (*result)->cNetwork.populations[popID].neurons.begin();
                                          neuron != (*result)->cNetwork.populations[popID].neurons.end();
                                          ++neuron) {
                for (vector<double>::iterator s = (*neuron)->spikes.begin(); s != (*neuron)->spikes.end(); ++s) {
                    if (*s == *i) {
                        ++count;
                    }
                }
            }
        }
    }
    return boost::tuple<vector<double>, vector<double> > (x, c);
}

boost::tuple< vector<double>, vector<double>, vector<double> > Results::meanSpikeCount(const string popID, const string ID) {
    // Return the mean spike counts over all simulations.
    // When the result is constrained to have zero parameters, this is
    // equivalent to mean spike count over all trials.
    vector<double> means;
    vector<double> err_up;
    vector<double> err_down;
    if (this->unconstrained.find(ID) == this->unconstrained.end()) {
        cout << "[X] Mean spike count requested on a parameter that is not unconstrained." << endl;
        return means; 
    }

    vector<double> counts;
    for (Range::iterator sIter = this->unconstrained[ID].begin(); sIter != this->unconstrained[ID].end(); ++sIter) {
        int total = 0;
        int count = 0;
        vector<Results::Result*> r = this->get(ID, *sIter);
        counts.clear();
        for (vector<Results::Result*>::iterator result = r.begin(); result != r.end(); ++result) {
            for (list<Neuron*>::iterator neuron = (*result)->cNetwork.populations[popID].neurons.begin();
                                          neuron != (*result)->cNetwork.populations[popID].neurons.end();
                                          ++neuron) {
                total += (*neuron)->spikes.size();
                counts.push_back((*neuron)->spikes.size());
                ++count; 
            }
        }
        if (count == 0) count = 1;
        means.push_back((double)total/(double)count);
        double se = stderror(counts);
        err_up.push_back(se);
        err_down.push_back(se);
    }

    return boost::tuple<vector<double>, vector<double>, vector<double> > (means, err_up, err_down);
}

void Results::add(Result &r) {
    results[results_size] = r; // Add it to the pile.
    this->filter.push_back(&results[results_size]); // Add on the last index
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
    if (type_ID == "population") {
        if (r.cNetwork.populations[item_ID].params.vals[param_ID] == value) matches = true;
    }
    if (type_ID == "results") {
        if (item_ID == "merge") {
            if (r.result_set == value) matches = true;
        }
    }

    return matches;
}

bool Results::merge(Results &r1, Results &r2) {
    this->T = r1.T;
    this->dt = r1.dt;
    this->delay = r1.delay;
    this->timeseries = r1.timeseries;
    this->filter.clear(); // Make sure we clear the filter and start fresh.

    int result_set = 1; // What to set the r2 result_set to.  Defaults to 1.
    bool set_first_result_set = true;
    if (r1.unconstrained.find("results.merge.set") != r1.unconstrained.end()) {
       result_set = r1.unconstrained["results.merge.set"].max()+1;
       set_first_result_set = false;
    }
    r1.unconstrained["results.merge.set"] = Range(0, result_set, 1);

    for (vector< Result* >::iterator i = r1.filter.begin(); i != r1.filter.end(); ++i) {
        if (set_first_result_set) {
            (*i)->result_set = 0;
        }
        this->filter.push_back(*i);
    }
    for (vector< Result* >::iterator i = r2.filter.begin(); i != r2.filter.end(); ++i) {
        (*i)->result_set = result_set;
        this->filter.push_back(*i);
    }

    this->unconstrained = r1.unconstrained;
    for (map<string, Range>::iterator i = r2.unconstrained.begin(); i != r2.unconstrained.end(); ++i) {
        if (r1.unconstrained.find(i->first) == r1.unconstrained.end()) this->unconstrained[i->first] = i->second;
        else this->unconstrained[i->first].merge(i->second); // Merge the two ranges
    }
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
    for (vector<Result*>::iterator i = this->filter.begin(); i != this->filter.end(); ++i) {
        if (this->matches(**i, ID, value)) { 
            r.filter.push_back(*i); 
        }
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

    // Load up the filter
    for (deque<Result>::iterator r_iter = r.results.begin(); r_iter != r.results.end(); ++r_iter) {
        r.filter.push_back(&(*r_iter));
    }
    return true;
}
