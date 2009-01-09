
#include "neuronparams.h"

NeuronParams::NeuronParams() : type(POISSON), integrator(Euler) {}
NeuronParams::NeuronParams(ModelType type) : type(type), integrator(RungeKutta) { initialize(); }
NeuronParams::NeuronParams(ModelType type, Integrator integrator) : type(type), integrator(integrator) { initialize(); }

void initialize() {
    switch(this->type) {
        case POISSON:
            vals["mu"] = Range(300);
            toggles["spontaneous"] = false;
            break;

        case AEIF:
            vals["VT"] = Range(-52);
            sigmas["VT"] = 0;

            vals["C"] = Range(281);
            sigmas["C"] = 0;

            vals["hypTau"] = Range(5);
            sigmas["hypTau"] = 0;

            vals["alpha_q"] = Range(1);
            sigmas["alpha_q"] = 0;

            vals["gL"] = Range(30);
            sigmas["gL"] = 0;

            vals["EL"] = Range(-63);
            sigmas["EL"] = 0;

            vals["tauw"] = Range(200);
            sigmas["tauw"] = 0;

            vals["a"] = Range(1500);
            sigmas["a"] = 0;

            vals["b"] = Range(80.5);
            sigmas["b"] = 0;

            vals["deltaT"] = Range(2);
            sigmas["deltaT"] = 0;

            vals["VR"] = Range(-63);
            sigmas["VR"] = 0;
            break;
    }
}
