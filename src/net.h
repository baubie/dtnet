
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

class Net {

    public:
        Net();
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

        struct ConstrainedNetwork {
            std::map<std::string, Population::ConstrainedPopulation> populations;    
            std::map< std::string, std::map< std::string, Connection<double> > > connections;

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

        std::vector<ConstrainedNetwork> cNetworks;
        void genNetworks();

        std::string filename;
		int numPopulations();                
        bool parseXML(std::string filename, std::string &error);
        void initialize();
};
#endif

