
#include "aEIF.h"

using namespace std;

extern "C" Neuron* create() {
    return new aEIF;
}
extern "C" void destroy(Neuron* n) {
    delete n;
}

void aEIF::initalize() {
    this->V = this->params.vals["EL"];
    this->w = 0;
}

void aEIF::update(double& current, unsigned int& position, double& dt) {
    this->V += this->diffsolve(&V_update, current, position, dt, this);
    this->w += this->diffsolve(&V_update, current, position, dt, this);
    this->spike(position, dt);
}

void aEIF::spike(unsigned int &position, double &dt) {
    static string s_VR = "VR";
    static string s_b = "b";

    if (this->V >= 20) {
        this->V = this->params.vals[s_VR];
        this->w += this->params.vals[s_b];
        voltage[position] = spike_height; // Artificial spike
        spikes.push_back(position*dt - this->delay); // Save the actual spike time
    }
}

double V_update(double& V, double& current, unsigned int& position, Neuron *n) {
    static string s_gL = "gL";
    static string s_EL = "EL";
    static string s_deltaT = "deltaT";
    static string s_VT = "VT";
    static string s_C = "C";

    double IL = n->params.vals[s_gL] * (n->V - n->params.vals[s_EL]);
    double ILd = -n->params.vals[s_gL] * n->params.vals[s_deltaT] * exp((n->V-n->params.vals[s_VT])/n->params.vals[s_deltaT]);
    double r =  (current - IL - ILd - static_cast<aEIF*>(n)->w) / n->params.vals[s_C];
    if (r > 10000) r = 10000; // Prevent overflows
    return r;
}

double w_update(Neuron *n) {
    static string s_a = "a";
    static string s_EL = "EL";
    static string s_tauw = "tauw";

    return (n->params.vals[s_a]*(n->V - n->params.vals[s_EL])-static_cast<aEIF*>(n)->w)/n->params.vals[s_tauw];
}

