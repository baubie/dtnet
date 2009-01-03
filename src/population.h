
#ifndef POPULATION_H
#define POPULATION_H

#include <string>
#include <sstream>
#include <vector>
#include "neuron.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

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
};
#endif
