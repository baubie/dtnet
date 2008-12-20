
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <vector>
#include "neuron.h"

using namespace std;

class Population {

    public:
		// Parameters
		string name;
		
		// Methods
		vector<Neuron> neurons;
		Population(string name, int size, NeuronParams params);
};
#endif
