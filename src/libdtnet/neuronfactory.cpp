
#include "neuronfactory.h"

/*
 * http://www.isotton.com/devel/docs/C++-dlopen-mini-HOWTO/C++-dlopen-mini-HOWTO.html#thesolution
 */

bool NeuronFactory::create(std::string model_type, NeuronParams* np, Neuron* &n) {

    std::transform(model_type.begin(), model_type.end(), model_type.begin(), (int(*)(int))tolower);

    std::string library_name = "libdtnet_" + model_type + ".so";

    void* handle = dlopen(library_name.c_str(), RTLD_NOW);
    if (!handle) {
        std::cerr << "Cannot load library: " << dlerror() << std::endl;
        return false;
    }

    // reset errors
    dlerror();

    // load the symbols
    create_t* create_model = (create_t*) dlsym(handle, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol create: " << dlsym_error << std::endl;
        std::cerr <<  "This does not appear to be a valid dtnet neuron model library." << std::endl;
        return false;
    }

    // create an instance of the class
    n = create_model();

        // copy in the parameters pass in if they are not null.
    if (np != NULL) n->params = *np;

    return true;
}

bool NeuronFactory::close() {

    std::map<std::string, void*>::iterator m_iter;

    for (m_iter = this->models.begin(); m_iter != this->models.end(); ++m_iter)
    {
        dlclose(m_iter->second);
    }

    return true;
}
