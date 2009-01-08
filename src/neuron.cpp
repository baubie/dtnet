
#include "neuron.h"

Neuron::Neuron(NeuronParams params) : params(params) {
    this->initialize();	
}

Neuron::Neuron() {
    this->params = Neuron::defaultParams();
    this->initialize();
}

void Neuron::initialize() {
    this->def_params = this->params;
	this->V = this->params.aEIF.EL; //mV
	this->w = 0;
}

NeuronParams Neuron::defaultParams() {
    NeuronParams p;
    
    // Global Parameters
    p.type = NeuronParams::AEIF;
    p.integrator = NeuronParams::RungeKutta;
    
    // Poisson
    p.Poisson.mu = 300; // In Hz
    p.Poisson.spontaneous = false;
        
    // AEIF
    p.aEIF.jitter = true;
    p.aEIF.VT = -52;
    p.aEIF.C	= 281;
    p.aEIF.hypTau = 5;
    p.aEIF.alpha_q = 1;
    p.aEIF.gL = 30;
    p.aEIF.EL = -63;
    p.aEIF.tauw = 200;
    p.aEIF.a = 1500;
    p.aEIF.deltaT = 2;
    p.aEIF.b = 80.5;
    p.aEIF.VR = -63;	

    p.aEIF.jC	= 0;
    p.aEIF.jVT = 0;
    p.aEIF.jhypTau = 0;
    p.aEIF.jalpha_q = 0;
    p.aEIF.jgL = 0;
    p.aEIF.jEL = 0;
    p.aEIF.jtauw = 0;
    p.aEIF.ja = 0;
    p.aEIF.jdeltaT = 0;
    p.aEIF.jb = 0;
    p.aEIF.jVR = 0;
    return p;
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
    switch(this->params.type) {
        case NeuronParams::AEIF:
            this->params.aEIF.C = n(this->def_params.aEIF.C, this->def_params.aEIF.jC);
            this->params.aEIF.VT = n(this->def_params.aEIF.VT, this->def_params.aEIF.jVT);
            this->params.aEIF.hypTau = n(this->def_params.aEIF.hypTau, this->def_params.aEIF.jhypTau);
            this->params.aEIF.alpha_q = n(this->def_params.aEIF.alpha_q, this->def_params.aEIF.jalpha_q);
            this->params.aEIF.gL = n(this->def_params.aEIF.gL, this->def_params.aEIF.jgL);    
            this->params.aEIF.EL = n(this->def_params.aEIF.EL, this->def_params.aEIF.jEL);
            this->params.aEIF.tauw = n(this->def_params.aEIF.tauw, this->def_params.aEIF.jtauw);
            this->params.aEIF.a = n(this->def_params.aEIF.a, this->def_params.aEIF.ja);
            this->params.aEIF.deltaT = n(this->def_params.aEIF.deltaT, this->def_params.aEIF.jdeltaT);
            this->params.aEIF.b = n(this->def_params.aEIF.b, this->def_params.aEIF.jb);
            this->params.aEIF.VR = n(this->def_params.aEIF.VR, this->def_params.aEIF.jVR);
        break;
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
    double mu = params.Poisson.mu;

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
                this->V = this->params.aEIF.VR;
                this->w += this->params.aEIF.b;
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
    IL = params.aEIF.gL * (V - params.aEIF.EL);
    ILd = -params.aEIF.gL * params.aEIF.deltaT * exp((V-params.aEIF.VT)/params.aEIF.deltaT);
    double r =  (current - IL - ILd - w) / params.aEIF.C;
    if (r > 100000) r = 100000; // Prevent overflows
    return (double)r;
}

double Neuron::w_update() {
    return (params.aEIF.a*(V-params.aEIF.EL)-w)/params.aEIF.tauw;
}
