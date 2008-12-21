
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
#include "lib/tinyxml/tinyxml.h"

class Net {

    private:
		float dt;
		float T;

        static const int ALPHA_WIDTH = 20; // 20 ms is MORE than enough width for the alpha function
        static const int CONSTANT_INPUT = 999;

		unsigned int steps;
		double alphaTauE;
        double alphaTauI;
		
        std::vector<Population> populations;
        std::vector< std::vector<double> > delays;
        std::vector< std::vector<double> > weights;

        std::vector< std::vector<double> > inputs;
        std::vector<double> alphaE;
        std::vector<double> alphaI;

        int createPopulation(std::string name, int size, NeuronParams params);
        int numPopulations();
        void initAlpha(float q, float tau, std::vector<double> &vals);
		double alpha(float t, std::vector<float> &spikes, double delay, double weight);
		void initSimulation();
		void finalizePopulations();
		void connectPopulations(int from, int to, double weight, double delay);
		void connectInput(int to, std::vector<double> input);
		void geninput(std::vector<double>* input, float duration, float mu, double delay);		
		void runSimulation();

    public:
        Net();
        bool verbose;

	
		void saveVoltages(std::string filename);
		void loadNetwork(std::string filename);		
};
#endif
