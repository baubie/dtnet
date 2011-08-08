
#include "aEIF.h"

BOOST_CLASS_EXPORT(aEIF)

using namespace std;

extern "C" Neuron* create() { return new aEIF; }
extern "C" void destroy(Neuron* n) { delete n; }
aEIF* aEIF::clone() { aEIF* r = new aEIF(*this); registerModel(r); return r;}

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
    return p;
}

void aEIF::initialize() {

    // Avoid map lookups during simulation
    this->C = this->params.getval("C");
    this->gL = this->params.getval("gL");
    this->EL = this->params.getval("EL");
    this->VT = this->params.getval("VT");
    this->deltaT = this->params.getval("deltaT");
    this->tauw = this->params.getval("tauw");
    this->a = this->params.getval("a");
    this->b = this->params.getval("b");
    this->VR = this->params.getval("VR");

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
        spikes.push_back(position * dt - this->delay); // Save the spike time
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

