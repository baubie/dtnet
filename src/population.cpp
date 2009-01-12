
#include "population.h"

using namespace std;


std::vector<Population::ConstrainedPopulation>* Population::populationFactory() {
    this->genPopulations();
    return &(this->cPopulations);
}

void Population::genPopulations() {

    // For now just produce a single constrained population.
    // TODO Create a vector of populations for each range value.
    ConstrainedPopulation cp;
    cp.ID = this->ID;
    cp.params = this->params;
    cp.neurons = vector<Neuron>(this->neurons);
    cp.accept_input = this->accept_input;
    this->cPopulations.push_back(cp);
}

Population::Population(string name, string ID, int size, bool accept_input, NeuronParams params) : name(name), ID(ID), accept_input(accept_input), params(params)  { 
    this->initialize(size, params);
}

Population::Population() : name(""), ID(""), accept_input(false) { 
    NeuronParams np; this->initialize(1, np); 
}

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
