
#include "HH.h"

BOOST_CLASS_EXPORT(HH)

        
using namespace std;
extern "C" Neuron* create() { return new HH; }
extern "C" void destroy(Neuron* n) { delete n; }
HH* HH::clone() { HH* r = new HH(*this); registerModel(r); return r; }

map<string, double> HH::default_parameters() {
    map<string, double> p;

    p["size"] = 1;

    p["E_Na"] = 45; // mV
    p["E_K"] = -80;
    p["E_T"] = 22;
    p["E_L"] = -70;

    p["g_Na"] = 43; // uS/cm^2
    p["g_K"] = 20;
    p["g_T"] = 0.55;
    p["g_L"] = 0.7;

    p["T"] = 6.3;

    p["alpha_q"] = 1;

    return p;
}

void HH::initialize() {

    n = 0;
    m = 0;
    h = 0;
    mT = 0;
    hT = 0;

    F = 96480; // Faraday's constant
    R = 8.314; // Gas constant
    T = this->params.getval("T"); // Temperature
    P_Ca = 0.0003;
    Ca_i = 3 * (10^(-8));
    Ca_o = 2 * (10^(-8));

    V_rest = -65;
    V = V_rest + 0.1;

    double a_m = 0.1 * (25 / (exp(2.5) - 1));
    double b_m = 4;
    double a_n = 0.01 * (10 / (exp(1) - 1));
    double b_n = 0.125;
    double a_h = 0.07;
    double b_h = 1 / (exp(3) + 1);

    m = a_m / (a_m + b_m);
    n = a_n / (a_n + b_n);
    h = a_h / (a_h + b_h);
    mT = 1 / (1 + exp(-(V_rest + 60) / 3));
    hT = 1 / (1 + exp(-(V_rest + 78) / 3.75));

    // Avoid map lookups during simulation
    this->E_Na = this->params.getval("E_Na");
    this->E_K = this->params.getval("E_K");
    this->E_L = this->params.getval("E_L");
    this->g_Na = this->params.getval("g_Na");
    this->g_K = this->params.getval("g_K");
    this->g_L = this->params.getval("g_L");

}

void HH::spike(unsigned int &position, double &dt) {
    if (this->V >= 20) {
        spikes.push_back(position * dt - this->delay); // Save the spike time
    }
}