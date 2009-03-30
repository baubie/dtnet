
#include "neuron.h"

using namespace std;

Neuron::Neuron(NeuronParams params) : params(params) {}
Neuron::Neuron() {}

void Neuron::init(int steps, double delay) {
    this->completeParameters();
    this->def_params = this->params;
    this->voltage.resize(steps, 0.0);
    this->delay = delay;
    this->jitter();
    this->initialize();
}

void Neuron::completeParameters() {
    // Fill in the parameters that are missing with defaults
    NeuronParams p = this->default_parameters();
    map<std::string, Range>::iterator v_iter;
    map<std::string, bool>::iterator t_iter;

    for (v_iter = p.vals.begin(); v_iter != p.vals.end(); ++v_iter) {
        if (this->params.vals.find(v_iter->first) == this->params.vals.end()) {
            this->params.vals[v_iter->first] = v_iter->second;
            this->params.sigmas[v_iter->first] = 0;
        }
    }
    for (t_iter = p.toggles.begin(); t_iter != p.toggles.end(); ++v_iter) {
        if (this->params.toggles.find(t_iter->first) == this->params.toggles.end()) {
            this->params.toggles[t_iter->first] = t_iter->second;
        }
    }
}

double n(double mean, double sigma) {
    if (sigma <= 0) return mean;
    extern boost::mt19937 random_engine;
    boost::normal_distribution<double> norm_dist(mean, sigma);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > generator(random_engine, norm_dist);
    return generator(); 
}

void Neuron::jitter() {
    for(map<string,Range>::iterator iter = this->params.vals.begin(); iter != this->params.vals.end(); ++iter) {
        if (this->params.sigmas.find(iter->first) != this->params.sigmas.end()) {
            this->params.vals[iter->first] = Range(n(this->def_params.vals[iter->first], this->def_params.sigmas[iter->first]));
        }
    }
}

double Neuron::diffsolve(double (*func)(double,double&,unsigned int&,Neuron*), double val, double &current, unsigned int &position, double &dt, Neuron *n) {
    return RungeKutta(func, val, current, position, dt, n);
}


double Neuron::RungeKutta(double (*func)(double,double&,unsigned int&,Neuron*), double val, double &current, unsigned int &position, double &dt, Neuron *n) {
    double r;
    double k1,k2,k3,k4;
    k1 = (*func)(val, current, position, n)*dt;
    k2 = (*func)(val+0.5*k1, current, position, n)*dt;
    k3 = (*func)(val+0.5*k2, current, position, n)*dt;
    k4 = (*func)(val+k3, current, position, n)*dt;
    r = (k1+2*k2+2*k3+k4)/6;
    return r;
}

/*
void Neuron::Spike(int position, double dt) {
    static string s_VR = "VR";
    static string s_b = "b";

    switch(this->params.type) {
        case NeuronParams::AEIF:
            if (this->V >= 20) {
                this->V = this->params.vals[s_VR];
                this->w += this->params.vals[s_b];
                voltage[position] = spikeHeight; // Artificial spike
                spikes.push_back(position*dt - this->delay); // Save the actual spike time
            }
            break;
        case NeuronParams::POISSON:
        // We assume this is only called when a spike actually occurred.
            this->voltage[position] = spikeHeight;
            spikes.push_back(position*dt - this->delay);
            break;
    }
}
 */

