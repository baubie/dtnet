#ifndef NEURONFACTORY_H
#define NEURONFACTORY_H

#include "neuron.h"
#include <iostream>
#include <dlfcn.h>
#include <map>
#include <algorithm>
#include <cctype>

class NeuronFactory {

    public:
        bool create(std::string model_type, NeuronParams *params, Neuron* &n);
        bool close();
        void registerModel(std::string model_type, Neuron* n);
        bool loadModel(std::string model_type);
        std::list<std::string> model_types();

        static NeuronFactory *s_instance;
        static NeuronFactory* instance()
        {
            if (!s_instance) s_instance = new NeuronFactory;
            return s_instance;
        }

    private:
        std::string getLibraryName(std::string model_type);
        std::map<Neuron*, std::string> models;
        std::map<std::string, void*> handles;
        std::list<std::string> _model_types;
};


#endif
