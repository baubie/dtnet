
#include "population.h"

using namespace std;

Population::Population(string name, string ID, int size, bool accept_input, NeuronParams params) : name(name), ID(ID), accept_input(accept_input) 
{ this->initialize(size, params); }

Population::Population() : name(""), ID(""), accept_input(false) 
{ this->initialize(1, Neuron::defaultParams()); }

void Population::initialize(int size, NeuronParams params) {
	for( int i = 0; i < size; i++ ) {
		neurons.push_back(Neuron(params));
	}
}

string Population::toString() {
    stringstream r;
    r << "Name: " << this->name << endl;
    r << "ID: " << this->ID << endl;
    r << "Size: " << this->neurons.size() << endl;
	r << "Accept Input: " << ((this->accept_input)?"Yes":"No") << endl;
    return r.str();
}
