
#include "simulation.h"

using namespace std;

Simulation::Simulation(Net &net) : net(net) {}

bool Simulation::linktrial(const Trial &trial, const string popID) {
    if (this->net.accept_input(popID)) {
        this->trials.insert(make_pair(popID, trial));
        return true;
    }
    cout << "[X] " << popID << " does not accept input." << endl;
    return false;
}

bool Simulation::run() {
    return true;
}

string Simulation::toString() {
	string r;
	r = "A Simulation";
	return r;
}
