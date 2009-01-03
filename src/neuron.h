
#ifndef NEURON_H
#define NEURON_H

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <boost/random.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


struct NeuronParams {

    enum Integrator { Euler, Euler2, RungeKutta } integrator;
    enum ModelType { AEIF, POISSON } type;

    struct POISSON {
        double mu;
        bool spontaneous;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & mu;
            ar & spontaneous;
        }
    } Poisson;

    struct AEIF {
        bool jitter;
        double VT;
        double C;
        double hypTau;
        double alpha_q;
        double gL;
        double EL;
        double tauw;
        double a;
        double deltaT;
        double b;
        double VR;

        // The sigma value of each parameter
        double jVT;
        double jC;
        double jhypTau;
        double jalpha_q;
        double jgL;
        double jEL;
        double jtauw;
        double ja;
        double jdeltaT;
        double jb;
        double jVR;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & jitter;
            ar & VT;
            ar & C;
            ar & hypTau;
            ar & alpha_q;
            ar & gL;
            ar & EL;
            ar & tauw;
            ar & a;
            ar & deltaT;
            ar & b;
            ar & VR;

            // The sigma value of each parameter
            ar & jVT;
            ar & jC;
            ar & jhypTau;
            ar & jalpha_q;
            ar & jgL;
            ar & jEL;
            ar & jtauw;
            ar & ja;
            ar & jdeltaT;
            ar & jb;
            ar & jVR;
        }
    } aEIF;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & Poisson;
        ar & aEIF;
    }
};

class Neuron {

	public:

		// Recording Variables
        std::vector<double> voltage;
        std::vector<double> spikes;
		
		// Methods
		Neuron(NeuronParams params);
        Neuron();
		static NeuronParams defaultParams();
		void init(int steps);
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

		double V;    // Voltage (mV)
		double w;
		
		static const int spikeHeight = -20;
			
		// Model Parameters
		NeuronParams params;		
		
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
