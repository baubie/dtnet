
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
                ar & name;
                ar & ID;
                ar & accept_input;
                ar & neurons;
            }
        };

        vector<ConstrainedPopulation>* populationFactory();

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & name;
            ar & ID;
            ar & accept_input;
            ar & neurons;
        }

        void initialize(int size, NeuronParams params);
        vector<ConstrainedPopulation> populations;
};
#endif
