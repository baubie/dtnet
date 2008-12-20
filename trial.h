
#ifndef TRIAL_H
#define TRIAL_H


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "input.h"
#include "lib/tinyxml/tinyxml.h"

using namespace std;

class Trial {
	
	private:
		vector<Input> inputs;
        vector<vector<double> > inputSignals;
        vector<double> timesteps;

		bool parseXML(string filename, string &error);
        void genSignal(vector<Input>::iterator inputsPos);
        void genTimeSeries();

        double T; // Total time of the trial simulation
        double dt; // The width of a single time step
        double delay; // A delay before time 0 in the trial

        string filename;
		
	public:	

        string name; // A name for the trial

        Trial(double T, double dt, double delay);
        Trial(const Trial &trial);
		bool load(string filename, string &error);		
        vector<vector<double> >* signals();
        vector<double>* timeSteps();
        int count();

        string toString();
	
};
#endif
