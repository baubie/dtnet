
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include <map>
#include "trial.h"
#include "net.h"

#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
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

        bool run(Results &r, std::string filename, double T, double dt, double delay, int number_of_trials, bool voltage, boost::threadpool::pool &tp);
		std::string toString();

        static bool simulationProgress(boost::threadpool::pool &tp, int total, boost::posix_time::ptime start);
        static const int ALPHA_WIDTH = 20; // 20 ms is MORE than enough width for the alpha function
        static double alpha(double t, std::vector<Neuron> &neurons, double tau, double delay, double globalDelay, double dt);
        static void runSimulation(Results::Result *r, double T, double dt, double delay, bool voltage);

    private:

        std::vector<double> genTimeSeries(double T, double dt, double delay);
};


#endif
