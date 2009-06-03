
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <list>
#include <string>
#include <map>
#include <fstream>
#include "trial.h"
#include "net.h"

#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "lib/threadpool/threadpool.hpp"
#include <boost/random.hpp>
#include <boost/bind.hpp>

#include "results.h"

/** Container class for holding multiple simulation runs. */
class Simulation {
public:

    /** Simulation Parameters **/
    Net net; /**< The network used in this simulation. */
    Trial trial; /**< The trial used in this simulation. */

    Simulation(Net &net, Trial &trial);
    Simulation();

    bool run(Results &r, std::string filename, double T, double dt, double delay, int number_of_trials, bool voltage, boost::threadpool::pool &tp);

    bool modify(std::string ID, Range const val);
    std::string toString();

    static bool simulationProgress(boost::threadpool::pool &tp, int total, boost::posix_time::ptime start);
    static void runSimulation(Results::Result *r, double T, double dt, double delay, bool voltage, bool quiet);

    bool quiet;

private:

    std::vector<double> genTimeSeries(double T, double dt, double delay);
};


#endif
