
#ifndef PHASELOCK_H
#define PHASELOCK_H

#include "../libdtnet/neuron.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>

// Derive our class based on the Neuron class
class PhaseLock : public Neuron {
public:
    // dtnet required functions
    virtual void update(double& current, unsigned int& position, double& dt);
    virtual void spike(unsigned int &position, double &dt);
    virtual PhaseLock* clone();

    // dtnet optional functions
    virtual void initialize();
    virtual std::map<std::string, double> default_parameters();


    // Custom variables to track
    static const double spike_height = 40;
    double mu;
	double lastSpike;

private:
    double active;

    friend class boost::serialization::access;
    template<class Archive> 
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<Neuron>(*this);
    }

};

#endif
