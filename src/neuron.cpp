
#include "neuron.h"

Neuron::Neuron(NeuronParams params) : params(params) {
    this->initialize();	
}

Neuron::Neuron() {
    this->initialize();
}

void Neuron::initialize() {
    this->def_params = this->params;
	this->V = this->params.aEIF.EL; //mV
	this->w = 0;
}



void Neuron::init(int steps, double delay) {
    this->voltage.resize(steps);
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
    for(map<string,double>::iterator iter = this->params.vals.begin(); iter != this->params.vals.end(); ++iter) {
        if (this->params.sigmas.find(iter->first) != this->params.sigmas.end()) {
            iter->second = n(this->def_params.vals[iter->first], this->def_params.sigmas[iter->first]);
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

    if (current == 0) { this->active = 0; return; }
    this->active += dt;
    double mu = this->params.vals["mu"];

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
    
    if (r < p) { 
        Spike(position, dt);
    } else {
        voltage[position] = -65;
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
    
    double k1 = V_update(V, current, position)*dt;
    double k2 = V_update(V+0.5*k1, current, position)*dt;
    double k3 = V_update(V+0.5*k2, current, position)*dt;
    double k4 = V_update(V+k3, current, position)*dt;
    V += (k1+2*k2+2*k3+k4)/6;
    
    voltage[position] = V;
    Spike(position, dt);
}

void Neuron::Spike(int position, double dt) {
    switch(this->params.type) {
        case NeuronParams::AEIF:
            if (this->V >= 20) {
                this->V = this->params.vals["VR"];
                this->w += this->params.vals["b"];
                voltage[position] = spikeHeight; // Artificial spike
                spikes.push_back(position*dt - this->delay); // Save the actual spike time
            }
            break;
        case NeuronParams::POISSON:
        // We assume this is only called when a spike actually occurred.
            this->voltage[position] = spikeHeight; // Artificial spike
            spikes.push_back(position*dt - this->delay);
            break;
    }
}

double Neuron::V_update(double V, double current, int position) {
        
    double IL;
    double ILd;	
    IL = params.vals["gL"] * (V - params.vals["EL"]);
    ILd = -params.vals["gL"] * params.vals["deltaT"] * exp((V-params.vals["VT"])/params.vals["deltaT"]);
    double r =  (current - IL - ILd - w) / params.aEIF.C;
    if (r > 100000) r = 100000; // Prevent overflows
    return (double)r;
}

double Neuron::w_update() {
    return (params.vals["a"]*(V-params.vals["EL"])-w)/params.vals["tauw"];
}
