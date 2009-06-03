
#include "HH.h"

BOOST_CLASS_EXPORT(HH)
        
using namespace std;
extern "C" Neuron* create() { return new HH; }
extern "C" void destroy(Neuron* n) { delete n; }
HH* HH::clone() { HH* r = new HH(*this); registerModel(r); return r; }

map<string, double> HH::default_parameters() {
    map<string, double> p;

    p["size"] = 1;

    p["E_Na"] = 50; // mV
    p["E_K"] = -77;
    p["E_T"] = 22;
    p["E_L"] = -54.4;

    p["g_Na"] = 120; // uS/cm^2
    p["g_K"] = 36;
    p["g_T"] = 0.55;
    p["g_L"] = 0.3;

	p["V_rest"] = -65;

    p["T"] = 6.3;

    p["cm"] = 1;
    p["alpha_q"] = 1;

    return p;
}

void HH::initialize() {

    F = 96480; // Faraday's constant
    R = 8.314; // Gas constant
    P_Ca = 0.0003;
    Ca_i = 3 * (10^(-8));
    Ca_o = 2 * (10^(-8));

    V_rest = this->params.getval("V_rest");
	V = V_rest;

    m = 0.05;
    n = 0.317;
    h = 0.6;

    // Avoid map lookups during simulation
    this->E_Na = this->params.getval("E_Na");
    this->E_K = this->params.getval("E_K");
    this->E_L = this->params.getval("E_L");
    this->g_Na = this->params.getval("g_Na");
    this->g_K = this->params.getval("g_K");
    this->g_L = this->params.getval("g_L");
    this->cm = this->params.getval("cm");
    this->T = this->params.getval("T"); // Temperature
	this->tf = pow(3.0,(this->T-6.3)/10); // Temperature Fix
    
}

void HH::update(double& current, unsigned int& position, double& dt) {

	// Update channel probabilities
    this->n += this->diffsolve(&n_update, this->V, current, position, dt, this);	
    this->m += this->diffsolve(&m_update, this->V, current, position, dt, this);	
    this->h += this->diffsolve(&h_update, this->V, current, position, dt, this);	

	// Calculate currents
	this->I_K = this->g_K * pow(this->n,4) * (this->E_K - this->V);
	this->I_Na = this->g_Na * pow(this->m,3) * this->h * (this->E_Na - this->V);
	this->I_L = this->g_L * (this->E_L - this->V);
	
	// Update voltage
    this->V += this->diffsolve(&V_update, this->V, current, position, dt, this);

    voltage[position] = this->V;
    this->spike(position, dt);
	this->V_last = this->V;
}

double V_update(double V, double& current, unsigned int& position, Neuron *n) {
	HH* hh = static_cast<HH*>(n);
	return (current + hh->I_Na + hh->I_K + hh->I_L) / hh->cm;
}

double n_update(double V, double& current, unsigned int& position, Neuron *n) {
	HH* hh = static_cast<HH*>(n);

	double a = hh->tf * 0.01 * (V+55) / (1-exp(-(V+55)/10));
	double b = hh->tf * 0.125 * exp(-(V+65)/80);

	return a * (1 - hh->n) - b * (hh->n);
}

double m_update(double V, double& current, unsigned int& position, Neuron *n) {
	HH* hh = static_cast<HH*>(n);

	double a = hh->tf * 0.1 * (V+40) / (1-exp(-(V+40)/10));
	double b = hh->tf * 4 * exp(-(V+65)/18);

	return a * (1 - hh->m) - b * (hh->m);
}

double h_update(double V, double& current, unsigned int& position, Neuron *n) {
	HH* hh = static_cast<HH*>(n);

	double a = hh->tf * 0.07 * exp(-(V+65)/20);
	double b = hh->tf * 1 / (1 + exp(-(V+35)/10));
	return a * (1 - hh->h) - b * (hh->h);
}


void HH::spike(unsigned int &position, double &dt) {
	double t = position * dt;
	if (this->V < 20) in_spike = false;
    if (this->V >= 20 && this->V >= this->V_last && !in_spike) {
		in_spike = true;
        spikes.push_back(t - this->delay); // Save the spike time
    }
}
