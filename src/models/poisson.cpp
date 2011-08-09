
#include "poisson.h"

BOOST_CLASS_EXPORT(Poisson)

using namespace std;

extern "C" Neuron* create() { return new Poisson; }
extern "C" void destroy(Neuron* n) { delete n; }
Poisson* Poisson::clone() { Poisson* r = new Poisson(*this); registerModel(r); return r;}

map<string, double> Poisson::default_parameters() {
    map<string, double> p;
    p["mu"] = 100;
    return p;
}

void Poisson::initialize() {
    this->mu = this->params.getval("mu");
	srand( time(NULL) );
}

void Poisson::update(double& current, unsigned int& position, double& dt) {

    if (current == 0) {
        voltage[position] = -65;
        this->active = 0;
        return;
    }

    this->active += dt;

    // Initial faster spiking rate
    double maximum = 1000;
    double initial_spike_length = 1.0;
    double half_initial_length = 1.0;
    double initial_spike_height = (double) maximum / (double) mu;
    double half_initial_spike_height = 1.0 + (initial_spike_height - 1.0) / 2.0;
    double currentMult = 1;

    if (this->active < initial_spike_length) {
        if (mu < 100) {
            currentMult = 1;
        } else if (mu > 500) {
            currentMult = initial_spike_height;
        } else {
            currentMult = 1.0 + (initial_spike_height - 1.0) * sqrt((mu - 100.0) / 400.0);
        }
    } else if (this->active < initial_spike_length + half_initial_length) {
        if (mu < 100) {
            currentMult = 1;
        } else if (mu > 500) {
            currentMult = half_initial_spike_height;
        } else {
            currentMult = 1.0 + (half_initial_spike_height - 1.0) * sqrt((mu - 100.0) / 400.0);
        }
    }

    // Use rand() to determine if we have a spike
    // We expect to spike at mu Hz.
    double r = (double) (rand() % 10001); // Random number in [0,10000]
    // Finds mu in ms^-1 (mu is given in Hz)
    // Then multiplies by the time step to find mu in terms of per time step.
    // This gives a probability of firing in this time step.
    // When dt = 0.05, this means that mu is limited to <= 20,000Hz
    double p = (mu * 10 * dt) * current * currentMult; /**< Probability of firing. */

    if (r < p) spike(position, dt);
    else voltage[position] = -65;

}

void Poisson::spike(unsigned int &position, double &dt) {
    // We assume this is only called when a spike actually occurred.
    this->voltage[position] = spike_height;
    spikes.push_back(position * dt - this->delay);
}

