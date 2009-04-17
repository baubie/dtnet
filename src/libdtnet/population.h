
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include "neuron.h"
#include "neuronfactory.h"
#include "serialization.h"
#include "debug.h"

class Population {
public:
    // Parameters
    std::string name;
    std::string ID;
    std::string model_type;
    double accept_input;
    bool spontaneous;
    int position;
    NeuronParams params;
    std::map< std::string, Range > unconstrained; /*<< Collection of unconstrained IDs. */
	
	// Used for GUI to layout populations
	int x, y;

    // Methods
    Population(std::string name, std::string ID, double accept_input, bool spontaneous, int position, std::string model_type, NeuronParams params);
    Population();
    std::string toString();

    struct ConstrainedPopulation {
        std::string ID;
        std::string name;
        NeuronParams params;
        std::list<Neuron*> neurons;
        std::string model_type;
        int position;
        double accept_input;
        bool spontaneous;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & ID;
            ar & name;
            ar & params;
            ar & model_type;
            ar & neurons;
            ar & position;
            ar & accept_input;
        }
        
    };

    void genPopulations(std::map< std::string, Range>::iterator param_in);
    std::vector<ConstrainedPopulation>* populationFactory();

private:
    std::vector<ConstrainedPopulation> cPopulations; /**< Constrained popuations. **/
    void genPopulations();
};
#endif
