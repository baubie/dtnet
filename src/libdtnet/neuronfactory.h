#ifndef NEURONFACTORY_H
#define NEURONFACTORY_H

#include "neuron.h"
#include <iostream>
#include <dlfcn.h>
#include <map>

class NeuronFactory {

    public:
        bool create(std::string model_type, Neuron* &n);

    private:
        std::map<std::string, void*> models;

};

#endif
