
#ifndef INPUT_H
#define INPUT_H


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "serialization.h"
#include "range.h"
#include "debug.h"

class Input {
	
	public:
		static const int PURE = 1;
		static const int PULSE = 2;

        std::string name;
        std::string ID;
        int type;

        /** Defines a single, constrained signal. **/
        struct Signal {
            double duration;
            double amplitude;
            double delay;
            std::string ID;
            std::vector<double> values;


            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & duration;
                ar & amplitude;
                ar & delay;
                ar & values;
                ar & ID;
            }

            
        };

        Range duration;
        Range amplitude;
        Range delay;

        Input();
        std::vector<Signal>* inputs(double T, double dt, double delay);
        std::string toString();
        std::map< std::string, Range > unconstrained;       /*<< Collection of unconstrained IDs. */

	private:
        std::vector<Signal> signals;
        void generateSignals(double T, double dt, double delay);
};
#endif
