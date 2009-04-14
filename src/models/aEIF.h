
#ifndef AEIF_H
#define AEIF_H

#include "../libdtnet/neuron.h"


// Derive our class based on the Neuron class
class aEIF : public Neuron {
public:
    // dtnet required functions
    virtual void update(double& current, unsigned int& position, double& dt);
    virtual void spike(unsigned int &position, double &dt);
    virtual aEIF* clone();

    // dtnet optional functions
    virtual void initialize();
    virtual std::map<std::string, double> default_parameters();

    // Custom variables to track
    double w;
    static const double spike_height = 40;

    // Parameters
    double C,gL,EL,VT,deltaT,tauw,a,b,VR,hypTau,alpha_q;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) { 
        ar & boost::serialization::base_object<Neuron>(*this);
    }
};

double V_update(double V, double& current, unsigned int& position, Neuron *n);
double w_update(double w, double& current, unsigned int& position, Neuron *n);



#endif
