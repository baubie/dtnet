
#include "simulation.h"

using namespace std;

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

bool Simulation::run(string filename, int number_of_trials) {

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

    // Loop over trials
    for (int i = 0; i < number_of_trials; ++i) {
        // Loop over the dynamicTrial
        for (signalIter = dynTrial.signals()->begin(); signalIter != dynTrial.signals()->end(); ++signalIter) {
            Net new_net(this->net); // Create a new network from our existing one.
            for (trialIter = this->trials.begin(); trialIter != this->trials.end(); ++trialIter) {
                if (trialIter->first != this->dynamicTrial) {
                    // Link the first input signal
                    new_net.linkinput( trialIter->second.signals()->at(0), trialIter->first );
                }
            } 
            new_net.linkinput( *signalIter, this->dynamicTrial );

            // Inputs are all loaded up so lets run the trial
            new_net.runSimulation();
        }
    }
    return true;
}

string Simulation::toString() {
	string r;
	r = "A Simulation";
	return r;
}
