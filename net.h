
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

using namespace std;

class Net {

    private:
		float dt;
		float T;

        static const int ALPHA_WIDTH = 20; // 20 ms is MORE than enough width for the alpha function
        static const int CONSTANT_INPUT = 999;

		unsigned int steps;
		double alphaTauE;
        double alphaTauI;
		
		vector<Population> populations;
        vector< vector<double> > delays;
        vector< vector<double> > weights;

		vector< vector<double> > inputs;
        vector<double> alphaE;
        vector<double> alphaI;

        int createPopulation(string name, int size, NeuronParams params);
        int numPopulations();
        void initAlpha(float q, float tau, vector<double> &vals);
		double alpha(float t, vector<float> &spikes, double delay, double weight);
		void initSimulation();
		void finalizePopulations();
		void connectPopulations(int from, int to, double weight, double delay);
		void connectInput(int to, vector<double> input);
		void geninput(vector<double>* input, float duration, float mu, double delay);		
		void runSimulation();

    public:
        Net();
        bool verbose;

	
		void saveVoltages(string filename);
		void loadNetwork(string filename);		
};
#endif
