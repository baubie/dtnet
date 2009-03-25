
#ifndef AEIF_H
#define AEIF_H

#include "../neuron.h"

// Derive our class based on the Neuron class
class Poisson: public Neuron {

    public:
        void update(double current, int position, double dt);

    private:


};



#endif
