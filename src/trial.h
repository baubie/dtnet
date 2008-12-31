
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

/** Load and generate the input signal vectors from an XML file. */
class Trial {

	public:	
        std::string name;                                       /**< A name for this trial. */
		std::string ID;											/**< A unique ID for this trial. */

        Trial(double T, double dt, double delay);               /**< Constructor for a new trial. */

		bool load(std::string filename, std::string &error);    /**< Load a trial from an XML file. */

        std::vector<std::vector<double> >* signals();           /**< Provide a pointer to the input signal vectors. */
        std::vector<double>* timeSteps();                       /**< Provide a pointer to the timesteps vector. */
        int count();                                            /**< Provide the number of input signals generated in this trial. */

        std::string toString();                                 /**< Provide a string representation of this trial. */
	
	private:
        std::vector<Input> inputs;                              /**< Collection of input signal definitions to generate vectors from. */
        std::vector<std::vector<double> > inputSignals;         /**< Collection of generated input signals. */
        std::vector<double> timesteps;                          /**< A vector of the timesteps. */

		bool parseXML(std::string filename, std::string &error);    /**< Parse the XML file. */
        void genSignal(std::vector<Input>::iterator inputsPos);     /**< Generate input signals from the collection of input definitions. */
        void genTimeSeries();                                       /**< Generate a time series based on T, dt, and delay. */

        double T;                   /**< Total time of the trial simulation. */
        double dt;                  /**< The width of a single time step. */
        double delay;               /**< A delay before time 0 in the trial. */
        std::string filename;       /**< XML from which this Trial was read from. */
};
#endif
