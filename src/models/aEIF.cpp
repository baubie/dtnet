
#include "aEIF.h"

using namespace std;

extern "C" Neuron* create() {
    return new aEIF;
}

extern "C" void destroy(Neuron* n) {
    delete n;
}

aEIF* aEIF::clone(std::string &name) {
    aEIF *r = new aEIF(*this);
    name = "aEIF";
    return r;
}

map<string,double> aEIF::default_parameters() {
    map<string, double> p;

    p["size"] = 1;

    // aEIF parameters
    p["C"] = 281;
    p["gL"] = 30;
    p["EL"] = -70.6;
    p["VT"] = -50.4;
    p["deltaT"] = 2;
    p["tauw"] = 144;
    p["a"] = 4;
    p["b"] = 0.0805;
    p["VR"] = -70.6;
    p["hypTau"] = 2;
    p["alpha_q"] = 1;
    return p;
}

void aEIF::initialize() {

    // Avoid map lookups during simulation
    this->C = this->params.vals["C"];
    this->gL = this->params.vals["gL"];
    this->EL = this->params.vals["EL"];
    this->VT = this->params.vals["VT"];
    this->deltaT = this->params.vals["deltaT"];
    this->tauw = this->params.vals["tauw"];
    this->a = this->params.vals["a"];
    this->b = this->params.vals["b"];
    this->VR = this->params.vals["VR"];
    this->hypTau = this->params.vals["hypTai"];
    this->alpha_q = this->params.vals["alpha_q"];

    this->V = this->EL;
    this->w = 0;
}

void aEIF::update(double& current, unsigned int& position, double& dt) {

    this->V += this->diffsolve(&V_update, this->V, current, position, dt, this);
    this->w += this->diffsolve(&w_update, this->w, current, position, dt, this);

    voltage[position] = this->V;
    this->spike(position, dt);
}

void aEIF::spike(unsigned int &position, double &dt) {

    if (this->V >= 20) {
        this->V = this->VR;
        this->w += this->b;
        voltage[position] = spike_height; // Artificial spike
        spikes.push_back(position * dt - this->delay); // Save the actual spike time
    }
}

double V_update(double V, double& current, unsigned int& position, Neuron *n) {
    aEIF* a = static_cast<aEIF*>(n);
    double IL = a->gL * (V - a->EL);
    double ILd = -a->gL * a->deltaT * exp((V - a->VT) / a->deltaT);
    double r = (current - IL - ILd - a->w) / a->C;
    if (r > 10000) r = 10000; // Prevent overflows
    return r;
}

double w_update(double w, double& current, unsigned int& position, Neuron *n) {
    aEIF* a = static_cast<aEIF*>(n);
    return (a->a * (n->V - a->EL) - w) / a->tauw;
}

