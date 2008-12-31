
#ifndef NET_H
#define NET_H

#include <string>
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

class Net {

    public:
        Net(double T, double dt);
        bool verbose;
        std::string name; // A name for this network

		void saveVoltages(std::string filename);
        void linkinput( std::vector<double> &input, const std::string popID );  
		bool load(std::string filename, std::string &error);	
        int count_populations();
        std::string toString();
		
        bool accept_input( const std::string popID );
        static const int NOT_FOUND = -999;

		std::vector<Population> populations;
		std::vector< std::vector<double> > delays;
		std::vector< std::vector<double> > weights;
		std::vector< std::vector<double> > sigmas;
		std::vector< std::vector<double> > density;
		std::vector< std::vector<double> > inputs;
		
		
    private:
		double dt;
		double T;

        static const int ALPHA_WIDTH = 20; // 20 ms is MORE than enough width for the alpha function
        static const int CONSTANT_INPUT = 999;

		unsigned int steps;
		double alphaTauE;
        double alphaTauI;
		

        std::vector<double> alphaE;
        std::vector<double> alphaI;
        
        std::string filename;

        void createPopulation(std::string name, std::string ID, int size, bool accept_input, NeuronParams params);
		int numPopulations();                
        int populationFromID(const std::string popID);
        void initAlpha(double q, double tau, std::vector<double> &vals);
		double alpha(double t, std::vector<double> &spikes, double delay, double weight);
		void initSimulation();
		void finalizePopulations();
		void connectPopulations(int from, int to, double weight, double delay);
		void geninput(std::vector<double>* input, double duration, double mu, double delay);		
		void runSimulation();

        bool parseXML(std::string filename, std::string &error);

};
#endif
