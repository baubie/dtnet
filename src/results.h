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

        std::map< std::string, Range > unconstrained;       /*<< Collection of unconstrained IDs. */
        std::vector<double> timeseries;                 /*<< Common timeseries of all simulations. */

        double dt;
        double T;
        double delay;

        bool use_external;
        std::vector< Result* > get();
        void setExternal(std::vector< Result> *ext);
        void add(Result &r);
        std::vector<int> filter;

        Results(double dt, double T, double delay);
        Results();
        std::string toString(); 
        static bool load(Results &r, std::string filename);
        void save(std::string filename);
        Results constrain(std::string ID, const double value);

    private:
        std::vector< Result > results;                  /*<< Collection of all results. */
        std::vector< Result > *external_results;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & unconstrained;
            ar & results;
        }
};

#endif
