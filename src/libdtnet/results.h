#ifndef RESULTS_H
#define RESULTS_H

#include "net.h"
#include "input.h"
#include "trial.h"
#include "serialization.h"
#include "range.h"
#include <math.h>
#include <map>
#include <deque>
#include <string>
#include <sstream>
#include <fstream>
#include "debug.h"
#include <boost/tuple/tuple.hpp>

class Results {
public:

    struct Result {
        int trial_num; /**< Keep track of which trial number this is. */
        int result_set; /**< When merging results, keep track of the different sets with this value. */

        /** Each result has ONE network and ONE trial. **/
        Net::ConstrainedNetwork cNetwork;
        Trial::ConstrainedTrial cTrial;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & trial_num;
            ar & result_set;
            ar & cNetwork;
            ar & cTrial;
        }

    };

    std::map< std::string, Range > unconstrained; /*<< Collection of unconstrained IDs. */
    std::vector<double> timeseries; /*<< Common timeseries of all simulations. */

    double dt;
    double T;
    double delay;

    std::vector< Result* > get();
    std::vector< Result* > get(const std::string ID, const double value);
    std::vector< Result* > filter;
    void add(Result &r);
    void init(int size);

    boost::tuple<std::vector<double>, std::vector<double>, std::vector<double> > meanSpikeCount(const std::string popID, const std::string ID);
    boost::tuple<std::vector<double>, std::vector<double>, std::vector<double> > firstSpikeLatency(const std::string popID, const std::string ID);
    boost::tuple<std::vector<double>, std::vector<double> > psth(const std::string popID, double window);

    Results(double dt, double T, double delay);
    Results();
    std::string toString();
    static bool load(Results &r, std::string filename);
    void save(std::string filename);
    bool constrain(Results &r, std::string ID, const double value);
    bool merge(Results &r1, Results &r2);
    bool matches(Result &r, std::string ID, const double value);

    int results_size;
    std::deque<Result> results; /*<< Collection of all results. */
private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & unconstrained;
        ar & timeseries;
        ar & results;
        ar & dt;
        ar & T;
        ar & delay;
        ar & delay;
    }
};

#endif
