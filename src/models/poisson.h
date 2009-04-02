
#ifndef POISSON_H
#define POISSON_H

#include "../libdtnet/neuron.h"
#include <math.h>

// Derive our class based on the Neuron class

class Poisson : public Neuron {
public:
    // dtnet required functions
    virtual void update(double& current, unsigned int& position, double& dt);
    virtual void spike(unsigned int &position, double &dt);
    virtual std::map<std::string, double> default_parameters();
    virtual Poisson* clone();

    // dtnet optional functions
    virtual void initialize();


    // Custom variables to track
    static const double spike_height = 40;
    double mu;

private:
    double active;

};



#endif
