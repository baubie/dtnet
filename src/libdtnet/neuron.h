
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
    NeuronParams params; /**< Parameters to run simulation with after jitter(). **/
    double V; /**< Voltage (mV) **/

    // Methods
    Neuron(NeuronParams params);
    Neuron();
    void init(int steps, double delay);

    // Virtual Methods
    // REQUIRED
    virtual void update(double &current, unsigned int &position, double &dt) = 0;
    virtual void spike(unsigned int &position, double &dt) = 0;
    virtual Neuron* clone(std::string &name) = 0;
    Neuron* clone();

    // OPTIONAL
    virtual void initialize() {};
    virtual std::map<std::string, double> default_parameters() { return std::map<std::string, double>(); }


protected:
    // Model Parameters
    NeuronParams def_params; /**< Parameters passed in before jittering. **/
    double active; /**< Used for Poisson to decide if it is currently active. **/
    double delay; /**< Global delay parameter (specifies time zero). **/

    // Differential Equation Solvers
    double diffsolve(double (*func)(double, double&, unsigned int&, Neuron*), double val, double &current, unsigned int &position, double &dt, Neuron* n);
    double RungeKutta(double (*func)(double, double&, unsigned int&, Neuron*), double val, double &current, unsigned int &position, double &dt, Neuron* n);

private:
    void jitter();
    void completeParameters();

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & voltage;
        ar & spikes;
        ar & params;
    }

};

// the types of the class factories
typedef Neuron* create_t();
typedef void destroy_t(Neuron*);

#endif
