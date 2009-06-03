#ifndef HH_H
#define	HH_H

#include "../libdtnet/neuron.h"

// Derive our class based on the Neuron class
class HH : public Neuron {
public:
    // dtnet required functions
    virtual void update(double& current, unsigned int& position, double& dt);
    virtual void spike(unsigned int &position, double &dt);
    virtual HH* clone();

    // dtnet optional functions
    virtual void initialize();
    virtual std::map<std::string, double> default_parameters();

    // Parameters
    double m,n,h,mT,hT,alpha_q;
    double F,R,T,P_Ca,Ca_i,Ca_o,cm;
    double E_Na,E_K,E_L;
    double g_Na,g_K,g_L;
    double V_rest;

    double tf;

	double I_Na, I_K, I_T, I_L;

private:
	double V_last;
	bool in_spike;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<Neuron> (*this);
    }

};

double V_update(double V, double& current, unsigned int& position, Neuron *n);


// Standard HH Equations
double n_update(double V, double& current, unsigned int& position, Neuron *n);
double m_update(double V, double& current, unsigned int& position, Neuron *n);
double h_update(double V, double& current, unsigned int& position, Neuron *n);

// Ca2+ T-Type Current Equations
double mT_update(double V, double& current, unsigned int& position, Neuron *n);
double hT_update(double V, double& current, unsigned int& position, Neuron *n);

#endif	/* HH_H */

