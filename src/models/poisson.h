
#ifndef POISSON_H
#define POISSON_H

#include <neuron.h>
#include <math.h>

// Derive our class based on the Neuron class

class Poisson : public Neuron {
public:
    // dtnet required functions
    void update(double& current, unsigned int& position, double& dt);
    void spike(unsigned int &position, double &dt);
    NeuronParams default_parameters();
    Poisson* clone(std::string &name);

    // dtnet optional functions
    void initialize();


    // Custom variables to track
    static const double spike_height = 40;
    double mu;

private:
    double active;

};



#endif
