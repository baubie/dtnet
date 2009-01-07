
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include "trial.h"
#include "net.h"

#include <fstream>
#include "lib/threadpool/threadpool.hpp"
#include <boost/random.hpp>
#include <boost/bind.hpp>
#include "serialization.h"

/** Container class for holding multiple simulation runs. */
class Simulation
{
    public:

		Simulation(Net &net);
        Simulation();
        bool linktrial(Trial &trial, const std::string popID);
        bool run(std::string filename, int number_of_trials, boost::threadpool::pool &tp);
		std::string toString();
		Net net;                                    /**< The network used in this simulation. */
        std::map<std::string, Trial> trials;      /**< Collection of trials linked to specific populations. */

        /** Accessed as results[ input index ][ trial index ] **/
        std::vector<std::vector<Net> > results;     /**< Collection of the networks post-simulation indexed by input. */

        void save(std::string filename);
        static bool load(Simulation &sim, std::string filename);        

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & net;
            ar & trials;
            ar & results;
            ar & dynamicTrial;
        }
    
        std::string dynamicTrial;           /**< Key to the trial with >1 inputs. */
};


#endif
