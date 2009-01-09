#ifndef NEURONPARAMS_H
#define NEURONPARAMS_H

#include "serialization.h"
#include "range.h"
#include <map>
#include <string>

class NeuronParams {

    public:
        enum Integrator { Euler, Euler2, RungeKutta } integrator;
        enum ModelType { AEIF, POISSON } type;

        NeuronParams();
        NeuronParams(ModelType type);
        NeuronParams(ModelType type, Integrator integrator);
        void initialize();

        std::map<std::string, Range> vals;
        std::map<std::string, double> sigmas;
        std::map<std::string, bool> toggles;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & vals;
            ar & sigmas;
            ar & toggles;
        }
};



#endif
