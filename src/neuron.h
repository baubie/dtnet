
#ifndef NEURON_H
#define NEURON_H

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <boost/random.hpp>

struct NeuronParams {
	
	// Model Selection
	enum ModelTypes { AEIF, POISSON };
	ModelTypes type;
	
	// Model Parameters (Poisson)
	float mu;
	
	// Model Parameters (AEIF)
	enum Integrators { Euler, Euler2, RungeKutta };
	Integrators integrator; 
	bool jitter;
	float VT;
	float C;
	float hypTau;
	float alpha_q;
	float gL;
	float EL;
	float tauw;
	float a;
	float deltaT;
	float b;
	float VR;

	int jVT;
	int jC;
	int jhypTau;
	int jalpha_q;
	int jgL;
	int jEL;
	int jtauw;
	int ja;
	int jdeltaT;
	int jb;
	int jVR;
};

class Neuron {

	private:
		float V;    // Voltage (mV)
		float w;
		
		static const int spikeHeight = -20;
			
		// Model Parameters
		NeuronParams params;		
		
		// Calculate next voltage change with different methods
		float V_update(float V, float current, int position);
		float w_update();
		void Spike(int position, float dt);
		void Euler(float current, int position, float dt);
		void Euler2(float current, int position, float dt);
		void RungeKutta(float current, int position, float dt);
	
		// Calculuate poisson spikes
		void Poisson(float current, int position, float dt);
		
	public:
		// Recording Variables
        std::vector<float> voltage;
        std::vector<float> spikes;
		
		// Methods
		Neuron(NeuronParams params);
		static NeuronParams defaultParams();
		void init(int steps);
		void jitter();
		void update(float current, int position, float dt);
};
#endif
