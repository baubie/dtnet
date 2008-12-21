
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <vector>
#include "neuron.h"

class Population {

    public:
		// Parameters
        std::string name;
		
		// Methods
        std::vector<Neuron> neurons;
		Population(std::string name, int size, NeuronParams params);
};
#endif
