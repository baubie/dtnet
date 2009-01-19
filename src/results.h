#ifndef RESULTS_H
#define RESULTS_H

#include "net.h"
#include "input.h"
#include "trial.h"
#include "serialization.h"
#include "range.h"
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "debug.h"

class Results {

    public:

        struct Result {
            int trial_num;                              /**< Keep track of which trial number this is. */

            /** Each result has ONE network and ONE trial. **/
            Net::ConstrainedNetwork cNetwork; 
            Trial::ConstrainedTrial cTrial;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & trial_num;
                ar & cNetwork;
                ar & cTrial;
            }
        };

        std::map< std::string, Range > unconstrained;   /*<< Collection of unconstrained IDs. */
        std::vector<double> timeseries;                 /*<< Common timeseries of all simulations. */

        double dt;
        double T;
        double delay;

        std::vector< Result* > get();
        std::vector< Result* > get(const std::string ID, const double value);
        void add(Result &r);
        void init(int size);
        std::vector<int> filter;

        Results(double dt, double T, double delay);
        Results();
        std::string toString(); 
        static bool load(Results &r, std::string filename);
        void save(std::string filename);
        bool constrain(Results &r, std::string ID, const double value);
        bool matches(Result &r, std::string ID, const double value);

        static std::vector<Result> results;                  /*<< Collection of all results. */
    private:

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & unconstrained;
            ar & timeseries;
            ar & results;
            ar & dt;
            ar & T;
            ar & delay;
            ar & filter;
        }
};

#endif
