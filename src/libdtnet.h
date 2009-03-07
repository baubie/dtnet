
#include <boost/fusion/include/io.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/tuple/tuple.hpp>
#include "lib/threadpool/threadpool.hpp"
#include "GLE.h"
#include "net.h"
#include "trial.h"
#include "vt100.h"
#include "simulation.h"
#include "results.h"
#include "settings.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>


namespace dtnet {

    bool run(Results &result, Simulation &sim, std::string filename, int number_of_trials, double delay, bool voltage, boost::threadpool::pool &tp);
    bool print(void* ptr, int const type);
    bool constrain(Results &result, Results *old_results, const std::string ID, const double value); 
    bool merge(Results &result, Results *r1, Results *r2);
    bool modsim(Simulation &sim, Simulation &old_sim, const std::string ID, Range const val);

    // Graphing functions
    // bool f_graphinputs(Trial &trial, string const &filename); // NO LONGER IMPLEMENTED
    bool graphnetwork(Results &results, std::string const &filename);
    bool graphtrial(int type, Results &results, int trial, std::string const &filename);
    bool graphpsth(Results &results, std::string const &popID, std::string const &filename);
    bool graphspiketrains(Results &results, std::string const &popID, int trials, double start, double end, std::string const &filename); 
    bool graphspikecounts(Results &results, std::string const &popID, std::string const &x_axis, std::string const &filename, int const type);
    bool graphfirstspikelatency(Results &results, std::string const &popID, std::string const &x_axis, std::string const &filename, int const type);

}
