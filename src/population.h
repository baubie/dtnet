
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <sstream>
#include <vector>
#include "neuron.h"
#include "serialization.h"

class Population {

    public:
		// Parameters
        std::string name;
        std::string ID;
        bool accept_input;
        std::vector<Neuron> neurons;
        NeuronParams params;
		
		// Methods
		Population(std::string name, std::string ID, int size, bool accept_input, NeuronParams params);
        Population();
		std::string toString();

        struct ConstrainedPopulation {
            std::string ID;
            NeuronParams params;
            std::vector<Neuron> neurons;
            bool accept_input;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & ID;
                ar & params;
                ar & neurons;
                ar & accept_input;
            }
        };

        std::vector<ConstrainedPopulation>* populationFactory();

    private:
        void genPopulations();
        void initialize(int size, NeuronParams params);
        std::vector<ConstrainedPopulation> cPopulations;
};
#endif
