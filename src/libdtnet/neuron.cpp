
#include "neuron.h"
#include "neuronfactory.h"

using namespace std;

Neuron::Neuron(NeuronParams params) : params(params) {}
Neuron::Neuron() {}


void Neuron::registerModel(Neuron* n) {
    NeuronFactory::instance()->registerModel(this->model, n);
}


void Neuron::init(int steps, double delay) {
    this->completeParameters();
    this->def_params = this->params;
    this->voltage.resize(steps, 0.0);
    this->delay = delay;
//    this->jitter();
    this->initialize();
}

void Neuron::completeParameters() {
    // Fill in the parameters that are missing with defaults
    map<std::string, double> p = this->default_parameters();
    map<std::string, double>::iterator v_iter;

    for (v_iter = p.begin(); v_iter != p.end(); ++v_iter) {
        if (this->params.vals.find(v_iter->first) == this->params.vals.end()) {
            this->params.vals[v_iter->first] = Range(v_iter->second);
            this->params.sigmas[v_iter->first] = 0;
        }
    }
}

double n(double mean, double sigma) {
    if (sigma <= 0) return mean;

	// Need to use "extern" here to that we use the same boost::mt19937
	// as declared in simulation.cpp
    extern boost::mt19937 random_engine;
    boost::normal_distribution<double> norm_dist(mean, sigma);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > generator(random_engine, norm_dist);
    return generator();
}

void Neuron::jitter() {
    for(map<string,Range>::iterator iter = this->params.vals.begin(); iter != this->params.vals.end(); ++iter) {
        if (this->params.sigmas.find(iter->first) != this->params.sigmas.end()) {
            this->params.vals[iter->first] = Range(n(this->def_params.vals[iter->first], this->def_params.sigmas[iter->first]));
        }
    }
}

double Neuron::diffsolve(double (*func)(double,double&,unsigned int&,Neuron*), double val, double &current, unsigned int &position, double &dt, Neuron *n) {
    return RungeKutta(func, val, current, position, dt, n);
}


double Neuron::RungeKutta(double (*func)(double,double&,unsigned int&,Neuron*), double val, double &current, unsigned int &position, double &dt, Neuron *n) {
    double r;
    double k1,k2,k3,k4;
    k1 = (*func)(val, current, position, n)*dt;
    k2 = (*func)(val+0.5*k1, current, position, n)*dt;
    k3 = (*func)(val+0.5*k2, current, position, n)*dt;
    k4 = (*func)(val+k3, current, position, n)*dt;
    r = (k1+2*k2+2*k3+k4)/6;
    return r;
}

