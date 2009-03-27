
#ifndef AEIF_H
#define AEIF_H

#include "../neuron.h"

// Derive our class based on the Neuron class
class aEIF: public Neuron {

    public:

        // dtnet required functions
        Neuron* create() { return new aEIF; }
        void update(double& current, unsigned int& position, double& dt);
        void spike(unsigned int &position, double &dt);

        // dtnet optional functions
        void initalize();

        // Custom variables to track
        double w;
        static const double spike_height = 40;
};

double V_update(double& V, double& current, unsigned int& position, Neuron *n);
double w_update(Neuron *n);


#endif
