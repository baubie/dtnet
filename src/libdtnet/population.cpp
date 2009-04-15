
#include "population.h"

using namespace std;

Population::Population(string name, 
                       string ID,
                       double accept_input,
                       bool spontaneous,
                       int position,
                       std::string model_type,
                       NeuronParams params)
        : name(name),
        ID(ID),
        accept_input(accept_input),
        spontaneous(spontaneous),
        position(position),
        model_type(model_type),
        params(params)   {}

Population::Population() : name(""), ID(""), accept_input(0) {}

std::vector<Population::ConstrainedPopulation>* Population::populationFactory() {

    this->genPopulations( this->params.vals.begin() );

    for (vector<ConstrainedPopulation>::iterator p = this->cPopulations.begin(); p != this->cPopulations.end(); ++p) {
        int size = 1;
        if (p->params.vals.find("size") != p->params.vals.end()) size = p->params.vals["size"];

        // Build a list of neurons
        p->neurons.clear();

        // Populate that list with neurons
        for (int i = 0; i < size; ++i)
        {
            Neuron *new_neuron = NULL;
            if (NeuronFactory::instance()->create(this->model_type, &this->params, new_neuron))
            {
                p->neurons.push_back(new_neuron);
            } else {
                cerr << "ERROR LOADING NEURONS." << endl;
            }
        }
    }

    return &(this->cPopulations);
}

void Population::genPopulations( map< string, Range>::iterator param_in )
{
    map< string, Range>::iterator param = param_in;

    if (param->second.size() > 1) {
        this->unconstrained["population." + this->ID + "." + param->first] = param->second;
    }

    ///////////////
    // BASE CASE //
    ///////////////
    if (param == --(this->params.vals.end())) {
        this->cPopulations.clear();

        // Base case is at the last parameter
        // We add on all the values
        for (Range::iterator i = param->second.begin(); i != param->second.end(); ++i) {
            Population::ConstrainedPopulation new_pop;
            new_pop.spontaneous = this->spontaneous;
            new_pop.accept_input = this->accept_input;
            new_pop.name = this->name;
            new_pop.ID = this->ID;
            new_pop.position = this->position;
            NeuronParams np;
            np.vals[param->first] = Range(*i); 
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
        this->genPopulations( param_in );

        vector<ConstrainedPopulation> oldPops = this->cPopulations;
        this->cPopulations.clear();
        for (vector<ConstrainedPopulation>::iterator old = oldPops.begin(); old != oldPops.end(); ++old) {
            for (Range::iterator i = param->second.begin(); i != param->second.end(); ++i) {
                ConstrainedPopulation new_pop = *old;
                new_pop.params.vals[param->first] = Range(*i); 
                this->cPopulations.push_back(new_pop);
            }
        }
    }
}

string Population::toString() {
    stringstream r;
    r << "Name: " << this->name << endl;
    r << "ID: " << this->ID << endl;
    r << "Accept Input: " << ((this->accept_input)?"Yes":"No") << endl;
    return r.str();
}
