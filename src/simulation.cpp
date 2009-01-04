
#include "simulation.h"

using namespace std;

/** Random Number Generator **/
boost::mt19937 random_engine;


Simulation::Simulation(Net &net) : net(net) {}

bool Simulation::linktrial(Trial &trial, const string popID) {
    if (this->net.accept_input(popID)) {
        if (trial.count() == 1 || this->dynamicTrial == popID || this->dynamicTrial == "") {
            this->trials.insert(make_pair(popID, trial));
            if (trial.count() > 1) this->dynamicTrial = popID;
        } else { cout << "[X] " << this->dynamicTrial << " already has a trial linked to it with multiple input signals.\n [X]  Only single inputs may be linked to other populations." << endl; }
        return true;
    }
    cout << "[X] " << popID << " does not accept input." << endl;
    return false;
}

void Simulation::save(string filename) {
    ofstream ofs(filename.c_str());
    boost::archive::text_oarchive oa(ofs);
    oa << *this;    
}

bool Simulation::load(Simulation &sim, string filename) {
    ifstream ifs(filename.c_str());
    if (ifs.fail()) {
        cout << "[X] Error opening " << filename << ".  Sorry." << endl;
        return false;
    }
    boost::archive::text_iarchive ia(ifs);
    ia >> sim;    
    return true;
}

void runSimulation(Net &net)
{
    net.runSimulation();
}

bool Simulation::run(string filename, int number_of_trials, boost::threadpool::pool &tp) {

    // Pick a population to loop over all the trials with.
    // If one population has multiple input signals we pick that one.
    // Else it doesn't matter.
    string loopPopulation;

    if (this->dynamicTrial == "") {
        cout << "[X] No trial has been linked to this network.\n [X] This is required at this time." << endl;
        return false;
    }

    Trial dynTrial(this->trials.find(this->dynamicTrial)->second);
    vector<vector<double> >::iterator signalIter;   
    map<string,Trial>::iterator trialIter;
    vector< vector<Net> >::iterator resultIter;

    int total = number_of_trials * dynTrial.signals()->size();
    int count = 0;

    // Loop over the dynamicTrial
    for (signalIter = dynTrial.signals()->begin(); signalIter != dynTrial.signals()->end(); ++signalIter) {
        vector<Net> new_trials;
        this->results.push_back( new_trials );

        // Loop over trials
        for (int i = 0; i < number_of_trials; ++i) {
            
            Net new_net(this->net); // Create a new network from our existing one.
            for (trialIter = this->trials.begin(); trialIter != this->trials.end(); ++trialIter) {
                if (trialIter->first != this->dynamicTrial) {
                    // Link the first input signal
                    new_net.linkinput( trialIter->second.signals()->at(0), trialIter->first );
                }
            } 
            new_net.linkinput( *signalIter, this->dynamicTrial );

            // Set everything up
            new_net.initSimulation();

            // Inputs are all loaded up so lets schedul the trial
            this->results.back().push_back(new_net);
            tp.schedule(boost::bind(&runSimulation, this->results.back().back()));
            ++count;
        }
    }

    tp.wait();
    if (filename != "") {
        cout << "Saving simulation..." << endl;
        Simulation::save(filename);
    } else { cout << "[WARNING] Simulation WAS NOT SAVED!" << endl; }
    return true;
}

string Simulation::toString() {
	string r;
	r = "A Simulation";
	return r;
}
