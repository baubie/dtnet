
#include "population.h"

Population::Population(string name, int size, NeuronParams params) {
    this->name = name;
	
	for( int i = 0; i < size; i++ ) {
		neurons.push_back(Neuron(params));
	}
}
