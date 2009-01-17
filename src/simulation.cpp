
#include "simulation.h"

using namespace std;

/** Random Number Generator **/
boost::mt19937 random_engine;

Simulation::Simulation(Net &net, Trial &trial) : net(net), trial(trial) {}

double Simulation::alpha(double t, vector<Neuron> &neurons, double tau, double delay, double globalDelay, double dt) {

    static map<double, vector<double> > alpha;
    static int alpha_steps = (int)(Simulation::ALPHA_WIDTH/dt);
    static double q = 1000.0;


    if (alpha.find(tau) == alpha.end()) {
        alpha[tau] = vector<double>(alpha_steps,0);
        double tau2 = pow(tau,2);
        for (int i=0; i < alpha_steps; i++) {
            alpha[tau][i] = q*((i*dt) * exp(-(i*dt)/tau)) / tau2;
        }
    }


	double current = 0;
	double spike;
    int step;

    for (vector<Neuron>::iterator n = neurons.begin(); n != neurons.end(); ++n) {
        for (vector<double>::iterator s = n->spikes.begin(); s != n->spikes.end(); ++s) {
            // delay is the axonal delay and this->delay is the global time zero delay
            spike = t-*s- delay - globalDelay;
            if (spike > 0) {
                step = (int)(spike/dt);
                if (spike < Simulation::ALPHA_WIDTH) {
                        current += alpha[tau][step];
                } 
                else { break; }
            }
        }
    }
	return current;
}

void Simulation::runSimulation(Results::Result *r, double T, double dt, double delay) {

	double input;
    double new_input;
    double tau;
    unsigned int steps = (unsigned int)(T/dt);

    map<string, Population::ConstrainedPopulation>::iterator cpIter;
    vector<Neuron>::iterator nIter;
    map<string, Net::Connection<double> >::iterator fromIter; 
    
	for (unsigned int ts=0; ts < steps; ++ts) { // Loop over time steps
        for (cpIter = r->cNetwork.populations.begin(); cpIter != r->cNetwork.populations.end(); ++cpIter) {
            for (nIter = cpIter->second.neurons.begin(); nIter != cpIter->second.neurons.end(); ++nIter) { // Loop over neurons

				input = 0.0;
				
				// Find spikes into our population				
                for (fromIter = r->cNetwork.connections[cpIter->second.ID].begin(); 
                     fromIter != r->cNetwork.connections[cpIter->second.ID].end(); 
                     ++fromIter) {
                        new_input = 0;
                        if (fromIter->second.weight > 0) tau = 0.7;
                        else tau = 1.1; 
                        new_input += Simulation::alpha(ts*dt, r->cNetwork.populations[fromIter->first].neurons, tau, fromIter->second.delay, delay, dt) * fromIter->second.weight;
                        input += new_input / (double)(r->cNetwork.populations[fromIter->first].neurons.size());
				}
				
				// Add our input signal in
                if (cpIter->second.accept_input) {
                    input += r->cTrial.values[ts];
                }
				// Update our neuron
                nIter->update(input, ts, dt);
			}
		}
	}
}

bool Simulation::simulationProgress(boost::threadpool::pool &tp, int total, boost::posix_time::ptime start) {

    boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());
    int pending = tp.pending();
    if (tp.pending() == 0) {
        cout << endl;
        return false;
    }
    double percent_done = (double)(total - pending) / (double)total;
    boost::posix_time::time_duration dur = now - start;
    double time_left = (double)dur.total_microseconds() / percent_done;
    boost::posix_time::ptime finished = now + boost::posix_time::microseconds(time_left);
    boost::posix_time::time_duration left = boost::posix_time::microseconds(time_left) - dur;
    cout << "\r[" << (int)(percent_done * 100) << "%] " << left << " remaining. Finished at " << finished << flush;
    return true;
}

bool Simulation::run(Results &results, string filename, double T, double dt, double delay, int number_of_trials, boost::threadpool::pool &tp) {

    vector<Trial::ConstrainedTrial>* inputs = this->trial.inputFactory(T,dt,delay);
    vector<Net::ConstrainedNetwork>* networks = this->net.networkFactory();
    vector<double> timesteps = this->genTimeSeries(T, dt, delay);

    int total = number_of_trials * inputs->size();
    int steps = (int)(T/dt);
    int count = 0;

    results = Results(T,dt,delay);
    results.timeseries = timesteps;
    /**************************
     * INITIALIZE SIMULATIONS *
     **************************/
    cout << "Initializing Simulations..." << endl;
    // Steal the unconstrained IDs from the trial and network.
    for(map<string, Range>::iterator iter = this->net.unconstrained.begin(); iter != this->net.unconstrained.end(); ++iter) {
        results.unconstrained[iter->first] = iter->second;
    }
    for(map<string, Range>::iterator iter = this->trial.unconstrained.begin(); iter != this->trial.unconstrained.end(); ++iter) {
        results.unconstrained[iter->first] = iter->second;
    }

    for (vector<Trial::ConstrainedTrial>::iterator t = inputs->begin(); t != inputs->end(); ++t) {
        for (vector<Net::ConstrainedNetwork>::iterator n = networks->begin(); n != networks->end(); ++n) {
            for (int i = 0; i < number_of_trials; ++i) {

                // Initialize the neurons
                map<string, Population::ConstrainedPopulation>::iterator pops;
                vector<Neuron>::iterator neurons;
                for (pops = n->populations.begin(); pops != n->populations.end(); ++pops) {
                    for (neurons = pops->second.neurons.begin(); neurons != pops->second.neurons.end(); ++neurons) {
                        neurons->init(steps, delay);
                    } 
                }

                // Create the Result container
                Results::Result r; 
                r.cNetwork = *n;
                r.cTrial = *t;
                r.trial_num = i;
                results.add(r);
            }
        }
    }


    /*******************
     * RUN SIMULATIONS *
     *******************/
    boost::posix_time::ptime start(boost::posix_time::microsec_clock::local_time());
    tp.schedule(boost::threadpool::looped_task_func(boost::bind(&Simulation::simulationProgress, tp, total, start), 500));
    cout << "Running Simulations..." << endl;
    vector<Results::Result*> results_to_run = results.get();
    for (vector<Results::Result*>::iterator iter=results_to_run.begin(); iter != results_to_run.end(); ++iter) {
        tp.schedule(boost::bind(&runSimulation, *iter, T, dt, delay));
    }

    tp.wait();
    boost::posix_time::ptime end(boost::posix_time::microsec_clock::local_time());
    boost::posix_time::time_duration dur = end - start;
    cout << "Completed in " << dur << endl;

    if (filename != "") {
        cout << "Saving simulation..." << endl;
        results.save(filename);
    } else { cout << "[WARNING] Simulation WAS NOT SAVED!" << endl; }
    return true;
}

string Simulation::toString() {
	string r;
	r = "A Simulation\n";
	return r;
}

vector<double> Simulation::genTimeSeries(double T, double dt, double delay) {
    vector<double> timesteps;
    unsigned int steps = (int)(T/dt);
    timesteps.resize(steps);
    for (unsigned int i = 0; i < steps; ++i) {
        timesteps[i] = i * dt - delay;
    }
    return timesteps;
}

