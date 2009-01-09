
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include <map>
#include "trial.h"
#include "net.h"

#include "lib/threadpool/threadpool.hpp"
#include <boost/random.hpp>
#include <boost/bind.hpp>
#include "results.h"

/** Container class for holding multiple simulation runs. */
class Simulation
{
    public:

        /** Simulation Parameters **/
		Net net;            /**< The network used in this simulation. */
        Trial trial;        /**< The trial used in this simulation. */

		Simulation(Net &net, Trial &trial);
        Simulation();

        bool run(Results &r, std::string filename, double T, double dt, double delay, int number_of_trials, boost::threadpool::pool &tp);
		std::string toString();

    private:
        std::vector<double> genTimeSeries(double T, double dt, double delay);
};


#endif
