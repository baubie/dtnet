
#include "population.h"

using namespace std;

Population::Population(string name, string ID, int size, bool accept_input, NeuronParams params) {
    this->name = name;
    this->ID = ID;
    this->accept_input = accept_input;
	
	for( int i = 0; i < size; i++ ) {
		neurons.push_back(Neuron(params));
	}
}

string Population::toString() {
    stringstream r;
    r << "Name: " << this->name << endl;
    r << "ID: " << this->ID << endl;
    r << "Size: " << this->neurons.size() << endl;
    return r.str();
}
