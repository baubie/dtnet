
#ifndef INPUT_H
#define INPUT_H


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>


class Input {
	
	public:
		static const int PURE = 1;
		static const int PULSE = 2;

        struct Range {
            Range() : start(0),end(0),step(1) {}
            float start;
            float end;
            float step;
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
        std::vector< std::vector<double> > signals;
        void generateSignals(double T, double dt, double delay);
};
#endif
