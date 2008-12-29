
#include "neuron.h"

Neuron::Neuron(NeuronParams params) {
	
	// Setup default model parameters
	this->params = params;

	// Do some other setup stuff
	switch (params.type) {
		case NeuronParams::AEIF:
			if (params.aEIF.jitter) { jitter(); }
			break;
		case NeuronParams::POISSON:
			break;
	}

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

    void Neuron::init(int steps) {
        this->voltage.resize(steps);
    }

	void Neuron::jitter() {
       
        //TODO: Replace this jitter with a Gaussian distribution 
        
        // Jitter by a percentage
        // [-x,+x] = rand() % (2x+1) - x
        /*
		double jit_C = rand() % (this->params.jC*2+1) - this->params.jC;
		double jit_VT = rand() % (this->params.jVT*2+1) - this->params.jVT;
		double jit_hypTau = rand() % (this->params.jhypTau*2+1) - this->params.jhypTau;
		double jit_alpha_q = rand() % (this->params.jalpha_q*2+1) - this->params.jalpha_q;
		double jit_gL = rand() % (this->params.jgL*2+1) - this->params.jgL;
		double jit_EL = rand() % (this->params.jEL*2+1) - this->params.jEL;
		double jit_tauw = rand() % (this->params.jtauw*2+1) - this->params.jtauw;
		double jit_a = rand() % (this->params.ja*2+1) - this->params.ja;
		double jit_deltaT = rand() % (this->params.jdeltaT*2+1) - this->params.jdeltaT;
		double jit_b = rand() % (this->params.jb*2+1) - this->params.jb;
		double jit_VR = rand() % (this->params.jVR*2+1) - this->params.jVR;

		this->params.C *= jit_C/100.0+1.0;
		this->params.VT *= jit_VT/100.0+1.0;
		this->params.hypTau *= jit_hypTau/100.0+1.0;
		this->params.alpha_q *= jit_alpha_q/100.0+1.0;
		this->params.gL *= jit_gL/100.0+1.0;
		this->params.EL *= jit_EL/100.0+1.0;
		this->params.tauw *= jit_tauw/100.0+1.0;
		this->params.a *= (jit_a/100.0+1.0);
		this->params.deltaT *= (jit_deltaT/100.0+1.0);
		this->params.b *= (jit_b/100.0+1.0);
		this->params.VR *= (jit_VR/100.0+1.0);
        */
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


        // Use rand() to determine if we have a spike
        // We expect to spike at mu Hz.
		double r = (double)(rand() % 10001); // Random number in [0,10000]

        // Finds mu in ms^-1 (mu is given in Hz) 
        // Then multiplies by the time step to find mu in terms of per time step.
        // This gives a probability of firing in this time step.
        // When dt = 0.05, this means that mu <= 20,000Hz, so we're good.
		double p = (params.Poisson.mu * 10 * dt); // Probability of firing. (10.0 = 1000.0/100.0)
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
		switch(params.type) {
			case NeuronParams::AEIF:
				if (V >= 20) {
					V = params.aEIF.VR;
					w += params.aEIF.b;
					voltage[position] = spikeHeight; // Artificial spike
					spikes.push_back(position*dt); // Save the actual spike time
				}
				break;
			case NeuronParams::POISSON:
			// We assume this is only called when a spike actually occurred.
				voltage[position] = spikeHeight; // Artificial spike
				spikes.push_back(position*dt);
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
