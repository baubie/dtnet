
#ifndef LIBDTNET_H
#define LIBDTNET_H

#define LIBDTNET_VERSION "libdtnet 1.9.3"

#include <boost/fusion/include/io.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/tuple/tuple.hpp>
#include "lib/threadpool/threadpool.hpp"
#include <boost/algorithm/string.hpp>

#include "GLE.h"
#include "net.h"
#include "trial.h"
#include "simulation.h"
#include "neuronfactory.h"
#include "results.h"
#include "settings.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <boost/program_options.hpp>

namespace dtnet {

    static const double DEFAULT = -9999999;
    static const int PLOT_VOLTAGE = 1;
    static const int PLOT_SPIKES = 2;
    static const int PLOT_FLAT = 1;
    static const int PLOT_3D = 2;
    static const int PLOT_MAP = 3;

    bool initialize();
    std::list<std::string> models();
    bool set_threads(int threads);
    bool set(const std::string var, double const val);
    bool set(const std::string var, std::string const val);  
    double get_dbl(const std::string var);
    std::string get_str(const std::string var);

    bool quit();
    bool load(Results &result, const std::string filename);
    bool run(Results &result, Simulation &sim, std::string filename, int number_of_trials, double delay, bool voltage);
    bool constrain(Results &result, Results *old_results, const std::string ID, const double value);
    bool merge(Results &result, Results *r1, Results *r2);
    bool modsim(Simulation &sim, Simulation &old_sim, const std::string ID, Range const val);
    std::string version();
    std::map<std::string, double> defaultModelParams(std::string model_type);

    // Graphing functions
    // bool f_graphinputs(Trial &trial, string const &filename); // NO LONGER IMPLEMENTED
    bool graphnetwork(Results &results, std::string const &filename);
    bool graphtrial(int type, Results &results, int trial, std::string const &filename);
    bool graphpsth(Results &results, std::string const &popID, std::string const &filename);
    bool graphspiketrains(Results &results, std::string const &popID, int trials, double start, double end, std::string const &filename);
    bool graphspikecounts(Results &results, std::string const &popID, std::string const &x_axis, std::string const &filename, int const type);
    bool graphfirstspikelatency(Results &results, std::string const &popID, std::string const &x_axis, std::string const &filename, int const type);

}

#endif
