
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "population.h"
#include "neuron.h"

/** Container class for holding multiple simulation runs. */
class Simulation
{

    public:


    private:
        std::vector< std::vector<Population> > populations;     /**< Collection of the network post-simulation. */
        std::vector< std::vector<double> > inputs;              /**< Collection of the inputs, indexed to correspond to the populations vector. */

};


#endif
