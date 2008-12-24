
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

class Trial {

	public:	
        std::string name; // A name for the trial

        Trial(double T, double dt, double delay);
		bool load(std::string filename, std::string &error);		
        std::vector<std::vector<double> >* signals();
        std::vector<double>* timeSteps();
        int count();

        std::string toString();
	
	private:
        std::vector<Input> inputs;
        std::vector<std::vector<double> > inputSignals;
        std::vector<double> timesteps;

		bool parseXML(std::string filename, std::string &error);
        void genSignal(std::vector<Input>::iterator inputsPos);
        void genTimeSeries();

        double T; // Total time of the trial simulation
        double dt; // The width of a single time step
        double delay; // A delay before time 0 in the trial

        std::string filename;
};
#endif
