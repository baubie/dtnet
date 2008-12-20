
#include "neuron.h"

Neuron::Neuron(NeuronParams params) {
	
	// Setup default model parameters
	this->params = params;

	// Do some other setup stuff
	switch (params.type) {
		case NeuronParams::AEIF:
			if (params.jitter) { jitter(); }
			break;
		case NeuronParams::POISSON:
			break;
	}

	this->V = this->params.EL; //mV
	this->w = 0;
}

    NeuronParams Neuron::defaultParams() {
		NeuronParams p;
        
		p.type = NeuronParams::AEIF;
        
        // Poisson
		p.mu = 300; // In Hz
        
        // AEIF
		p.jitter = true;
		p.integrator = NeuronParams::RungeKutta;
		p.VT = -52;
		p.C	= 281;
		p.hypTau = 5;
		p.alpha_q = 1;
		p.gL = 30;
		p.EL = -63;
		p.tauw = 200;
		p.a = 1500;
		p.deltaT = 2;
		p.b = 80.5;
		p.VR = -63;	

		p.jC	= 0;
		p.jVT = 0;
		p.jhypTau = 0;
		p.jalpha_q = 0;
		p.jgL = 0;
		p.jEL = 0;
		p.jtauw = 0;
		p.ja = 0;
		p.jdeltaT = 0;
		p.jb = 0;
		p.jVR = 0;
		return p;
	}

    void Neuron::init(int steps) {
        this->voltage.resize(steps);
    }

	void Neuron::jitter() {
       
        // Jitter by a percentage
        // [-x,+x] = rand() % (2x+1) - x
		float jit_C = rand() % (this->params.jC*2+1) - this->params.jC;
		float jit_VT = rand() % (this->params.jVT*2+1) - this->params.jVT;
		float jit_hypTau = rand() % (this->params.jhypTau*2+1) - this->params.jhypTau;
		float jit_alpha_q = rand() % (this->params.jalpha_q*2+1) - this->params.jalpha_q;
		float jit_gL = rand() % (this->params.jgL*2+1) - this->params.jgL;
		float jit_EL = rand() % (this->params.jEL*2+1) - this->params.jEL;
		float jit_tauw = rand() % (this->params.jtauw*2+1) - this->params.jtauw;
		float jit_a = rand() % (this->params.ja*2+1) - this->params.ja;
		float jit_deltaT = rand() % (this->params.jdeltaT*2+1) - this->params.jdeltaT;
		float jit_b = rand() % (this->params.jb*2+1) - this->params.jb;
		float jit_VR = rand() % (this->params.jVR*2+1) - this->params.jVR;

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
	}

	void Neuron::update(float current, int position, float dt) {
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

	void Neuron::Poisson(float current, int position, float dt) {


        // Use rand() to determine if we have a spike
        // We expect to spike at mu Hz.
		double r = (double)(rand() % 10001); // Random number in [0,10000]

        // Finds mu in ms^-1 (mu is given in Hz) 
        // Then multiplies by the time step to find mu in terms of per time step.
        // This gives a probability of firing in this time step.
        // When dt = 0.05, this means that mu <= 20,000Hz, so we're good.
		double p = (params.mu * 10 * dt); // Probability of firing. (10.0 = 1000.0/100.0)
		p *= current; // Decrease or increase depending on current;
        
		if (r < p) { 
			Spike(position, dt);
		} else {
			voltage[position] = -65;
		}
	}

	void Neuron::Euler(float current, int position, float dt) {
		w += w_update() * dt;
		V += V_update(V, current, position) * dt;
        
		voltage[position] = V;
		Spike(position, dt);
	}

	void Neuron::Euler2(float current, int position, float dt) {
		w += w_update() * dt; // Just stick with Euler
        
		float Vtmp = V + V_update(V, current, position) * dt; // Trial step
		V += 0.5 * (V_update(V, current, position) + V_update(Vtmp, current, position)) * dt;
        
		voltage[position] = V;
		Spike(position, dt);
	}

	void Neuron::RungeKutta(float current, int position, float dt) {
		w += w_update() * dt; // Just stick with Euler
        
		float k1 = V_update(V, current, position)*dt;
		float k2 = V_update(V+0.5*k1, current, position)*dt;
		float k3 = V_update(V+0.5*k2, current, position)*dt;
		float k4 = V_update(V+k3, current, position)*dt;
		V += (k1+2*k2+2*k3+k4)/6;
        
		voltage[position] = V;
		Spike(position, dt);
	}

	void Neuron::Spike(int position, float dt) {
		switch(params.type) {
			case NeuronParams::AEIF:
				if (V >= 20) {
					V = params.VR;
					w += params.b;
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

	float Neuron::V_update(float V, float current, int position) {
			
		float IL;
		float ILd;	
		IL = params.gL * (V - params.EL);
		ILd = -params.gL * params.deltaT * exp((V-params.VT)/params.deltaT);
		double r =  (current - IL - ILd - w) / params.C;
		if (r > 100000) r = 100000; // Prevent overflows
		return (float)r;
	}

	float Neuron::w_update() {
		return (params.a*(V-params.EL)-w)/params.tauw;
	}
