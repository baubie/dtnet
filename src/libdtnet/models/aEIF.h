
#ifndef AEIF_H
#define AEIF_H

#include "../neuron.h"

// Derive our class based on the Neuron class
class aEIF: public Neuron {

    public:

        // dtnet required functions
        Neuron* create() { return new aEIF; }
        void update(double& current, int& position, double& dt);

    private:
        static const double spike_height = 40;
	double V_update(double& V, double& current, int& position);
	double w_update();
        double w;

};



#endif
