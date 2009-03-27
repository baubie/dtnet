
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

void aEIF::update(double& current, int& position, double& dt) {

}

double aEIF::V_update(double& V, double& current, int& position) {
    static string s_gL = "gL";
    static string s_EL = "EL";
    static string s_deltaT = "deltaT";
    static string s_VT = "VT";
    static string s_C = "C";

    double IL = params.vals[s_gL] * (V - params.vals[s_EL]);
    double ILd = -params.vals[s_gL] * params.vals[s_deltaT] * exp((V-params.vals[s_VT])/params.vals[s_deltaT]);
    double r =  (current - IL - ILd - w) / params.vals[s_C];
    if (r > 10000) r = 10000; // Prevent overflows
    return r;
}

double aEIF::w_update() {
    static string s_a = "a";
    static string s_EL = "EL";
    static string s_tauw = "tauw";
    return (params.vals[s_a]*(V-params.vals[s_EL])-w)/params.vals[s_tauw];
}
