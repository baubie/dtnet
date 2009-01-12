
#ifndef NEURON_H
#define NEURON_H

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <boost/random/normal_distribution.hpp>
#include <boost/random.hpp>
#include "serialization.h"
#include "neuronparams.h"
#include "range.h"

class Neuron {

	public:
		// Recording Variables
        std::vector<double> voltage;
        std::vector<double> spikes;
		
		// Methods
		Neuron(NeuronParams params);
        Neuron();
		void init(int steps, double delay);
		void jitter();
		void update(double current, int position, double dt);

	private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & voltage;
            ar & spikes;
            ar & params;
        }

		
		static const int spikeHeight = -20;
			
		// Model Parameters
		NeuronParams params;        /**< Parameters to run simulation with after jitter(). **/
		NeuronParams def_params;    /**< Parameters passed in before jittering. **/	
        double active;                /**< Used for Poisson to decide if it is currently active. **/
		double V;                   /**< Voltage (mV) **/
		double w;                   /**< Adaptation value for aEIF. **/
        double delay;               /**< Global delay parameter (specifies time zero). **/
		
		// Calculate next voltage change with different methods
		double V_update(double V, double current, int position);
		double w_update();
		void Spike(int position, double dt);
		void Euler(double current, int position, double dt);
		void Euler2(double current, int position, double dt);
		void RungeKutta(double current, int position, double dt);
	
		// Calculuate poisson spikes
		void Poisson(double current, int position, double dt);

        void initialize();
};
#endif
