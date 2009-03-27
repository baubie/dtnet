#ifndef NEURONFACTORY_H
#define NEURONFACTORY_H

#include "neuron.h"
#include <iostream>
#include <dlfcn.h>
#include <map>

class NeuronFactory {

    public:
        bool create(std::string model_type, NeuronParams *params, Neuron* &n);
        bool close();


    private:
        std::map<std::string, void*> models_c;
};

#endif
