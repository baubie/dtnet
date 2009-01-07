
#ifndef INPUT_H
#define INPUT_H


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "serialization.h"


class Input {
	
	public:
		static const int PURE = 1;
		static const int PULSE = 2;

        struct Range {
            Range() : start(0),end(0),step(1) {}
            float start;
            float end;
            float step;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & start;
                ar & end;
                ar & step;
            }
        };

        std::string name;
        int type;
        Range duration;
        Range amplitude;
        Range delay;

        Input();
        std::vector<std::vector<double> >* inputs(double T, double dt, double delay);
        std::string toString();


	private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & name;
            ar & type;
            ar & duration;
            ar & amplitude;
            ar & delay;
            ar & signals;
        }

        std::vector< std::vector<double> > signals;
        void generateSignals(double T, double dt, double delay);
};
#endif
