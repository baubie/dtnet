
#include "neuronfactory.h"

Neuron* NeuronFactory::create(std::string model_type) {

    /* TODO
        - Convert model_type into library name.
        - i.e.  "aEIF" -> "libdtnet_aeif.so"
        - check if a handle already exists
        - if not, load the file with dlopen() and save the handle
        - return a pointer to the library's create() function
    */


}
