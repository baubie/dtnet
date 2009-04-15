
#include "neuronfactory.h"

/*
 * http://www.isotton.com/devel/docs/C++-dlopen-mini-HOWTO/C++-dlopen-mini-HOWTO.html#thesolution
 */

std::list<std::string> NeuronFactory::model_types() {
    return this->_model_types;
}


bool NeuronFactory::loadModel(std::string model_type) {

    std::string library_name = getLibraryName(model_type);

    void* handle = dlopen(library_name.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot load " << library_name << ": " << dlerror() << std::endl;
        return false;
    }
    _model_types.push_back(model_type);
    handles[library_name] = handle;

}

void NeuronFactory::registerModel(std::string model_type, Neuron* n) {    
    std::string library_name = getLibraryName(model_type);
    this->models[n] = library_name;
}

bool NeuronFactory::create(std::string model_type, NeuronParams* np, Neuron* &n) {
    std::string library_name = getLibraryName(model_type);
	
    void* handle;

    if (handles.find(library_name) != handles.end()) {
        handle = handles.find(library_name)->second;
    } else {
        std::cerr << "Unknown Model Type: \"" << model_type << "\"" << std::endl;
        return false;
    }

    // reset errors
    dlerror();

    // load the symbols
    create_t* create_model = (create_t*) dlsym(handle, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol create: " << dlsym_error << std::endl;
        std::cerr << "This does not appear to be a valid dtnet neuron model library." << std::endl;
        return false;
    }

    // create an instance of the class
    n = create_model();

    // copy in the parameters pass in if they are not null.
    if (np != NULL) n->params = *np;
    n->model = model_type;

    this->models[n] = library_name;


    return true;
}

bool NeuronFactory::close() {

    std::map<Neuron*, std::string>::iterator n_iter;

    for (n_iter = this->models.begin(); n_iter != this->models.end(); ++n_iter) {
        void* handle = dlopen(n_iter->second.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Cannot load library: " << dlerror() << std::endl;
            return false;
        }

        // reset errors
        dlerror();

        // load the symbols
        destroy_t* destroy_model = (destroy_t*) dlsym(handle, "destroy");
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Cannot load symbol create: " << dlsym_error << std::endl;
            std::cerr << "This does not appear to be a valid dtnet neuron model library." << std::endl;
            return false;
        }

        destroy_model(n_iter->first);
    }

    std::map<std::string, void*>::iterator h_iter;
    for (h_iter = this->handles.begin(); h_iter != this->handles.end(); ++h_iter) {
        dlclose(h_iter->second);
    }

    return true;
}

std::string NeuronFactory::getLibraryName(std::string model_type) {
    std::string library_name;
    std::transform(model_type.begin(), model_type.end(), model_type.begin(), (int(*)(int))tolower);

#ifdef __APPLE__
    library_name = "libdtnet_" + model_type + ".dylib";
#else
    library_name = "libdtnet_" + model_type + ".so";
#endif

    return library_name;
}
