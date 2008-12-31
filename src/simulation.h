
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include "trial.h"
#include "net.h"

/** Container class for holding multiple simulation runs. */
class Simulation
{
    public:
		Simulation(Net &net);
        bool linktrial(const Trial &trial, const std::string popID);
        bool run();
		std::string toString();
		Net net;                                     /**< The network used in this simulation. */
        std::map< std::string, Trial > trials;        /**< Collection of trials linked to specific populations. */
        std::map< int, std::vector<Net> > results;    /**< Collection of the networks post-simulation indexed by input. */
	
};


#endif
