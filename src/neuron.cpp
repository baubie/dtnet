
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

void Neuron::Poisson(double current, int position, double dt) {

    static string s_mu = "mu";

    if (current == 0) { voltage[position] = -65; this->active = 0; return; }

    this->active += dt;
    double mu = this->params.vals[s_mu];

    // Initial faster spiking rate
    double maximum = 1000;
    double initial_spike_length = 1.0;
    double half_initial_length = 1.0;
    double initial_spike_height = (double)maximum/(double)mu;
    double half_initial_spike_height = 1.0 + (initial_spike_height-1.0)/2.0;
    double currentMult = 1;

    if (this->active < initial_spike_length) {
        if (mu < 100) { currentMult = 1; }
        else if (mu > 500) { currentMult = initial_spike_height; }
        else { currentMult = 1.0 + (initial_spike_height-1.0)*sqrt((mu-100.0)/400.0); }
    }
    else if (this->active < initial_spike_length + half_initial_length) {
        if (mu < 100) { currentMult = 1; }
        else if (mu > 500) { currentMult = half_initial_spike_height; }
        else { currentMult = 1.0 + (half_initial_spike_height-1.0)*sqrt((mu-100.0)/400.0); }
    }
    current *= currentMult;

    // Use rand() to determine if we have a spike
    // We expect to spike at mu Hz.
    double r = (double)(rand() % 10001); // Random number in [0,10000]
    // Finds mu in ms^-1 (mu is given in Hz) 
    // Then multiplies by the time step to find mu in terms of per time step.
    // This gives a probability of firing in this time step.
    // When dt = 0.05, this means that mu <= 20,000Hz, so we're good.
    double p = (mu * 10 * dt); /**< Probability of firing. */
    p *= current; // Decrease or increase depending on current;
    
    if (r < p) Spike(position, dt);
    else voltage[position] = -65;
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

double Neuron::V_update(double V, double current, int position) {

    static string s_gL = "gL";
    static string s_EL = "EL";
    static string s_deltaT = "deltaT";
    static string s_VT = "VT";
    static string s_C = "C";

    double IL = params.vals[s_gL] * (V - params.vals[s_EL]);
    double ILd = -params.vals[s_gL] * params.vals[s_deltaT] * exp((V-params.vals[s_VT])/params.vals[s_deltaT]);
    double r =  (current - IL - ILd - w) / params.vals[s_C];
    if (r > 10000) r = 10000; // Prevent overflows
    return r;
}

double Neuron::w_update() {
    static string s_a = "a";
    static string s_EL = "EL";
    static string s_tauw = "tauw";
    return (params.vals[s_a]*(V-params.vals[s_EL])-w)/params.vals[s_tauw];
}
