
#include "aEIF.h"

using namespace std;

extern "C" Neuron* create() {
    return new aEIF;
}

extern "C" void destroy(Neuron* n) {
    delete n;
}

aEIF* aEIF::clone() const {
    aEIF *r = new aEIF(*this);
    NeuronFactory::instance()->registerModel("aEIF", r);
    return r;
}

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

