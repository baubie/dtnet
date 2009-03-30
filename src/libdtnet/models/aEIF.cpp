
#include "aEIF.h"

using namespace std;

extern "C" Neuron* create() { return new aEIF; }
extern "C" void destroy(Neuron* n) { delete n;}
aEIF* aEIF::clone() const { return new aEIF(*this); }


NeuronParams aEIF::default_parameters() {
    NeuronParams p;

    p.vals["size"] = Range(1);

    // aEIF parameters
    p.vals["C"] = Range(281);
    p.vals["gL"] = Range(30);
    p.vals["EL"] = Range(-70.6);
    p.vals["VT"] = Range(-50.4);
    p.vals["deltaT"] = Range(2);
    p.vals["tauw"] = Range(144);
    p.vals["a"] = Range(4);
    p.vals["b"] = Range(0.0805);
    p.vals["VR"] = Range(-70.6);

    p.vals["hypTau"] = Range(2);
    p.vals["alpha_q"] = Range(1);
    return p;
}

void aEIF::initialize() {
    this->V = this->params.vals["EL"];
    this->w = 0;
}

void aEIF::update(double& current, unsigned int& position, double& dt) {

    this->V += this->diffsolve(&V_update, this->V, current, position, dt, this);
    this->w += this->diffsolve(&w_update, this->w, current, position, dt, this);

    voltage[position] = this->V;
    this->spike(position, dt);
}

void aEIF::spike(unsigned int &position, double &dt) {
    static string s_VR = "VR";
    static string s_b = "b";

    if (this->V >= 20) {
        this->V = this->params.vals[s_VR];
        this->w += this->params.vals[s_b];
        voltage[position] = spike_height; // Artificial spike
        spikes.push_back(position * dt - this->delay); // Save the actual spike time
    }
}

double V_update(double V, double& current, unsigned int& position, Neuron *n) {
    static string s_gL = "gL";
    static string s_EL = "EL";
    static string s_deltaT = "deltaT";
    static string s_VT = "VT";
    static string s_C = "C";

    double IL = n->params.vals[s_gL] * (V - n->params.vals[s_EL]);
    double ILd = -n->params.vals[s_gL] * n->params.vals[s_deltaT] * exp((V - n->params.vals[s_VT]) / n->params.vals[s_deltaT]);
    double r = (current - IL - ILd - static_cast<aEIF*> (n)->w) / n->params.vals[s_C];
    if (r > 10000) r = 10000; // Prevent overflows

    return r;
}

double w_update(double w, double& current, unsigned int& position, Neuron *n) {
    static string s_a = "a";
    static string s_EL = "EL";
    static string s_tauw = "tauw";

    return (n->params.vals[s_a]*(n->V - n->params.vals[s_EL]) - w) / n->params.vals[s_tauw];
}

