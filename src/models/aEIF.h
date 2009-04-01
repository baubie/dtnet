
#ifndef AEIF_H
#define AEIF_H

#include <neuron.h>

// Derive our class based on the Neuron class

class aEIF : public Neuron {
public:
    // dtnet required functions
    void update(double& current, unsigned int& position, double& dt);
    void spike(unsigned int &position, double &dt);
    NeuronParams default_parameters();
    aEIF* clone(std::string &name);

    // dtnet optional functions
    void initialize();

    // Custom variables to track
    double w;
    static const double spike_height = 40;

    // Parameters
    double C,gL,EL,VT,deltaT,tauw,a,b,VR,hypTau,alpha_q;
};

double V_update(double V, double& current, unsigned int& position, Neuron *n);
double w_update(double w, double& current, unsigned int& position, Neuron *n);


#endif
