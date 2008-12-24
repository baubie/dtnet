
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <sstream>
#include <vector>
#include "neuron.h"

class Population {

    public:
		// Parameters
        std::string name;
        std::string ID;
		
		// Methods
        std::vector<Neuron> neurons;
		Population(std::string name, std::string ID, int size, NeuronParams params);
		std::string toString();
};
#endif
