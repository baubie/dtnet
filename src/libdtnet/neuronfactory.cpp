
#include "neuronfactory.h"

/*
 * http://www.isotton.com/devel/docs/C++-dlopen-mini-HOWTO/C++-dlopen-mini-HOWTO.html#thesolution
 */

bool NeuronFactory::create(std::string model_type, Neuron* &n) {

    std::string library_name = "libdtnet_" + model_type + ".so";

    void* handle = dlopen(library_name.c_str(), RTDL_NOW);
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



    dlclose(handle);
}
