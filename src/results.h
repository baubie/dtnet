#ifndef RESULTS_H
#define RESULTS_H

#include "net.h"
#include "input.h"
#include "serialization.h"
#include <map>
#include <string>
#include <sstream>
#include <fstream>

class Results {

    public:
        struct Result {
            std::map< std::string, double > params;     /**< Map of parameters and their values. */
            std::map< std::string, Input > inputs;      /**< Map of inputs with populations as keys. */
            Net net;                                    /**< The network with results. */
            int trial_num;                              /**< Keep track of which trial number this is. */

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & params;
                ar & inputs;
                ar & net;
                ar & trial_num;
            }
        };

        std::vector< std::string > unconstrained;       /*<< Collection of unconstrained IDs. */
        std::vector< Result > results;                  /*<< Collection of all results. */
        std::vector<double> timeseries;                 /*<< Common timeseries of all simulations. */

        double dt;
        double T;
        double delay;

        std::string toString(); 
        static bool load(Results &r, std::string filename);
        void save(std::string filename);
        bool constrain( Results &r, const std::string popID, const double value);

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & unconstrained;
            ar & results;
        }
};

#endif
