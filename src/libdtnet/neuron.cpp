
#include "neuron.h"

using namespace std;

Neuron::Neuron(NeuronParams params) : params(params) {
    this->initialize();	
}

Neuron::Neuron() {
    this->initialize();
}

void Neuron::initialize() {
    this->def_params = this->params;
	this->V = -65; //mV
	this->w = 0;

    if (this->params.vals.find("EL") != this->params.vals.end()) this->V = this->params.vals["EL"];
}

void Neuron::init(int steps, double delay) {
    this->voltage.resize(steps, 0.0);
    this->delay = delay;
    this->jitter();
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

void Neuron::update(double current, int position, double dt) {
    switch(params.type) {
        
        case NeuronParams::AEIF:
            switch(params.integrator) {
                case NeuronParams::Euler:
                    Euler(current, position, dt);
                    break;
                case NeuronParams::Euler2:
                    Euler2(current, position, dt);
                    break;
                case NeuronParams::RungeKutta:
                    RungeKutta(current, position, dt);
                    break;
            }
            break;
        
        case NeuronParams::POISSON:
            Poisson(current, position, dt);
            break;
    }
    
}

void Neuron::Euler(double current, int position, double dt) {
    w += w_update() * dt;
    V += V_update(V, current, position) * dt;
    
    voltage[position] = V;
    Spike(position, dt);
}

void Neuron::Euler2(double current, int position, double dt) {
    w += w_update() * dt; // Just stick with Euler
    
    double Vtmp = V + V_update(V, current, position) * dt; // Trial step
    V += 0.5 * (V_update(V, current, position) + V_update(Vtmp, current, position)) * dt;
    
    voltage[position] = V;
    Spike(position, dt);
}

void Neuron::RungeKutta(double current, int position, double dt) {
    w += w_update() * dt; // Just stick with Euler
    
    double k1,k2,k3,k4;
    k1 = V_update(V, current, position)*dt;
    k2 = V_update(V+0.5*k1, current, position)*dt;
    k3 = V_update(V+0.5*k2, current, position)*dt;
    k4 = V_update(V+k3, current, position)*dt;
    V += (k1+2*k2+2*k3+k4)/6;

    voltage[position] = V;
    Spike(position, dt);
}

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

