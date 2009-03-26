#ifndef NEURONFACTORY_H
#define NEURONFACTORY_H

#include "neuron.h"
#include "map.h"

class NeuronFactory {

    public:
        Neuron* create(std::string model_type);

    private:
        std::map<std::string, void*> models;

};

#endif
