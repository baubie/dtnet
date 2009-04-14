
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
    double F,R,T,P_Ca,Ca_i,Ca_o;
    double E_Na,E_K,E_L;
    double g_Na,g_K,g_L;
    double V_rest;

private:
    double active;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<Neuron> (*this);
    }

};

#endif	/* HH_H */

