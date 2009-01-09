
#include "simulation.h"

using namespace std;

/** Random Number Generator **/
boost::mt19937 random_engine;

Simulation::Simulation(Net &net, Trial &trial) : net(net), trial(trial) {}

void runSimulation(Net *net) { net->runSimulation(); }

bool Simulation::run(Results &r, string filename, double T, double dt, double delay, int number_of_trials, boost::threadpool::pool &tp) {


    vector<Trial::CombinedInputs>* inputs = this->trial.inputFactory(T,dt,delay);
    vector<double> timesteps = this->genTimeSeries(T, dt, delay);

    int total = number_of_trials * inputs->size();
    int count = 0;

    vector<Net> simulations;


    /**************************
     * INITIALIZE SIMULATIONS *
     **************************/
    cout << "Initializing Simulations..." << endl;
    /** Steal the unconstrained IDs from the trial and network. **/
    for(vector<string>::iterator iter = this->net.unconstrained.begin(); iter != this->net.unconstrained.end(); ++iter) {
        r.unconstrained.push_back(*iter);
    }
    for(vector<string>::iterator iter = this->trial.unconstrained.begin(); iter != this->trial.unconstrained.end(); ++iter) {
        r.unconstrained.push_back(*iter);
    }
    // Initialize the base simulation
    this->net.initSimulation(T,dt,delay);





    /*******************
     * RUN SIMULATIONS *
     *******************/
    /*
    cout << "Running Simulations..." << endl;
    for (vector<Net>::iterator iter=simulations.begin(); iter != simulations.end(); ++iter) {
        tp.schedule(boost::bind(&runSimulation, &(*iter)));
    }

    tp.wait();

    if (filename != "") {
        cout << "Saving simulation..." << endl;
        r.save(filename);
    } else { cout << "[WARNING] Simulation WAS NOT SAVED!" << endl; }
    */ cout << "SKIPPING SIMULATION" << endl;
    return true;
}

string Simulation::toString() {
	string r;
	r = "A Simulation";
	return r;
}

vector<double> Simulation::genTimeSeries(double T, double dt, double delay) {
    vector<double> timesteps;
    unsigned int steps = (int)(T/dt);
    timesteps.resize(steps);
    for (unsigned int i = 0; i < steps; ++i) {
        timesteps[i] = i * dt - delay;
    }
}
