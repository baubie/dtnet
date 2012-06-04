
#include "phaselock.h"

BOOST_CLASS_EXPORT(PhaseLock)

using namespace std;

extern "C" Neuron* create() { return new PhaseLock; }
extern "C" void destroy(Neuron* n) { delete n; }
PhaseLock* PhaseLock::clone() { PhaseLock* r = new PhaseLock(*this); registerModel(r); return r;}

map<string, double> PhaseLock::default_parameters() {
    map<string, double> p;
    p["mu"] = 100;
    return p;
}

void PhaseLock::initialize() {
	this->lastSpike = 0;
    this->mu = this->params.getval("mu");
}

void PhaseLock::update(double& current, unsigned int& position, double& dt) {

    if (current == 0) {
        voltage[position] = -65;
        this->active = 0;
        return;
    }

	if (position*dt - this->lastSpike >= 1/(this->mu*0.001)) {
		spike(position, dt);
		this->lastSpike = position*dt;
	}
    else voltage[position] = -65;

}

void PhaseLock::spike(unsigned int &position, double &dt) {
    // We assume this is only called when a spike actually occurred.
    this->voltage[position] = spike_height;
    spikes.push_back(position * dt - this->delay);
}

