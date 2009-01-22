
#include "population.h"

using namespace std;


std::vector<Population::ConstrainedPopulation>* Population::populationFactory() {
    this->genPopulations();
    //TODO: Initialize neurons in each cPopulation
    return &(this->cPopulations);
}

void Population::genPopulations( map< string, Range>::iterator param_in )
{

    map< string, Range>::iterator param = param_in;

    ///////////////
    // BASE CASE //
    ///////////////
    if (param == --(param->second.end())) {
        this->cPopulations.clear();

        // Base case is at the last parameter
        // We add on all the values
        for (Range::iterator i = param->second.begin(); i != param->second.end(); ++i) {
            Population::ConstrainedPopulation new_pop;
            new_pop.spontaneous = this->spontaneous;
            new_pop.accept_input = this->accept_input;
            new_pop.name = this->name;
            new_pop.ID = this->ID;
            NeuronParams np;
            np.vals[param->first] = *i; 
            np.toggles = this->params.toggles;
            np.sigmas = this->params.sigmas;
            new_pop.params = np;
            this->cPopulations.push_back(new_pop);
        }
    }


    ////////////////////
    // RECURSIVE CASE //
    ////////////////////
    else {
        ++param_in;
        genPopulations( param_in );

        vector<ConstrainedPopulation> oldPops = this->cPopulations;
        this->cPopulations.clear();

        for (vector<ConstrainedPopulation>::iterator old = oldPops.begin(); old != oldPops.end(); ++old) {
            for (Range::iterator i = param->second.begin(); i != param->second.end(); ++i) {
                Population::ConstrainedPopulation new_pop = *old;
                NeuronParams np = old->params;
                np.vals[param->first] = *i; 
                new_pop.params = np;
                this->cPopulations.push_back(new_pop);
            }
        }
    }
}

Population::Population(string name, string ID, bool accept_input, bool spontaneous, int position, NeuronParams params) : name(name), ID(ID), accept_input(accept_input), spontaneous(spontaneous), position(position), params(params)  { 
    this->initialize(params);
}

Population::Population() : name(""), ID(""), accept_input(false) { 
    NeuronParams np; this->initialize(np); 
}

void Population::initialize(NeuronParams params) {
    int size = 1;
    if (params.vals.find("size") != params.vals.end()) size = params.vals["size"];
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
