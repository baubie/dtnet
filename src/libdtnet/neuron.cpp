
#include "neuron.h"

using namespace std;

Neuron::Neuron(NeuronParams params) : params(params) {}

void Neuron::init(int steps, double delay) {
    this->def_params = this->params;
    this->voltage.resize(steps, 0.0);
    this->delay = delay;
    this->jitter();
    this->initialize();
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

double Neuron::RungeKutta(double (*func)(double,double,double), double &current, int &position, double &dt) {
    double r;
    double k1,k2,k3,k4;
    k1 = func(V, current, position)*dt;
    k2 = func(V+0.5*k1, current, position)*dt;
    k3 = func(V+0.5*k2, current, position)*dt;
    k4 = func(V+k3, current, position)*dt;
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

