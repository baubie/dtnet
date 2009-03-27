
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
#include "debug.h"

class Neuron {

    public:
    // Recording Variables
        std::vector<double> voltage;
        std::vector<double> spikes;
        NeuronParams params;        /**< Parameters to run simulation with after jitter(). **/

        // Methods
        Neuron(NeuronParams params);
        Neuron();
        void init(int steps, double delay);
        void jitter();

        // Virtual Methods
        virtual void update(double &current, int &position, double &dt) {}
        virtual void initialize() {}

    protected:
        // Model Parameters
        NeuronParams def_params;    /**< Parameters passed in before jittering. **/
        double active;              /**< Used for Poisson to decide if it is currently active. **/
        double V;                   /**< Voltage (mV) **/
        double delay;               /**< Global delay parameter (specifies time zero). **/

        // Differential Equation Solvers
        void Euler(double& current, int& position, double& dt);
        void Euler2(double& current, int& position, double& dt);
        void RungeKutta(double& current, int& position, double& dt);

        // Calculate next voltage change with different methods
        void Spike(int position, double dt);


    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & voltage;
            ar & spikes;
            ar & params;
        }

};

// the types of the class factories
typedef Neuron* create_t();
typedef void destroy_t(Neuron*);

#endif
