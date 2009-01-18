
#ifndef NET_H
#define NET_H

#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "population.h"
#include "neuron.h"
#include "lib/tinyxml/tinyxml.h"
#include "serialization.h"
#include "range.h"
#include "debug.h"

class Net {

    public:
        bool verbose;
        std::string name; // A name for this network

        void linkinput( std::vector<double> &input );  
		bool load(std::string filename, std::string &error);	
        int count_populations();
        std::string toString();
		void initSimulation(double T, double dt, double delay);

        template<class T>
        struct Connection {
            T delay;
            T weight;
            T density;
            T sigma;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & delay;
                ar & weight;
                ar & density;
                ar & sigma;
            }
        };

        static bool pop_order_sort ( Population::ConstrainedPopulation* a, Population::ConstrainedPopulation* b ) {
            return (a->position < b->position);
        }

        struct ConstrainedNetwork {
            std::map<std::string, Population::ConstrainedPopulation> populations;    
            std::map< std::string, std::map< std::string, Connection<double> > > connections;

            std::vector<Population::ConstrainedPopulation*> popvector() {
                std::vector<Population::ConstrainedPopulation*> r;
                for (std::map<std::string, Population::ConstrainedPopulation>::iterator i = this->populations.begin(); i != this->populations.end(); ++i) {
                    r.push_back(&(i->second));
                }
                std::sort(r.begin(), r.end(), Net::pop_order_sort);
                return r;
            }

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & populations;
                ar & connections;
            }
        };

        std::vector<ConstrainedNetwork>* networkFactory();
		
        bool accept_input( const std::string popID );
        static const int NOT_FOUND = -999;

        std::map< std::string, Range > unconstrained;

		std::map<std::string, Population> populations;
        std::map< std::string, std::map< std::string, Connection<Range> > > connections;

    private:

        void genConnections( std::map< std::string, std::map< std::string, Connection<Range> > >::iterator to,
                             std::map< std::string, Connection<Range> >::iterator from ); 

        std::vector< std::map< std::string, Population::ConstrainedPopulation > > cPopulations; /**< Constrained popuations. **/
        std::vector< std::map< std::string, std::map< std::string, Connection<double> > > > cConnections; /**< Constrained connections. **/
        std::vector<ConstrainedNetwork> cNetworks; /**< Completed, constrained networks taking all combinations of cPopulations and cConnections. **/

        void genNetworks();
        std::string filename;
		int numPopulations();                
        bool parseXML(std::string filename, std::string &error);
        void initialize();
};
#endif

