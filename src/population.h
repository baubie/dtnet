
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <sstream>
#include <vector>
#include "neuron.h"
#include "serialization.h"
#include "debug.h"

class Population {

    public:
		// Parameters
        std::string name;
        std::string ID;
        bool accept_input;
        bool spontaneous;
        int position;
        std::vector<Neuron> neurons;
        NeuronParams params;
		
		// Methods
		Population(std::string name, std::string ID, bool accept_input, bool spontaneous, int position, NeuronParams params);
        Population();
		std::string toString();


        struct ConstrainedPopulation {
            std::string ID;
            std::string name;
            NeuronParams params;
            std::vector<Neuron> neurons;
            int position;
            bool accept_input;
            bool spontaneous;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & ID;
                ar & name;
                ar & params;
                ar & neurons;
                ar & position;
                ar & accept_input;
            }
        };

        void genPopulations( std::map< std::string, Population::ConstrainedPopulation >::iterator pop_in,
                             std::map< std::string, Range>::iterator param_in );
        std::vector<ConstrainedPopulation>* populationFactory();

    private:
        std::vector<ConstrainedPopulation> cPopulations; /**< Constrained popuations. **/
        void genPopulations();
        void initialize(int size, NeuronParams params);
};
#endif
