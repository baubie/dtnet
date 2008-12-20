
#include "input.h"

using namespace std;

Input::Input() {
    Range blank;
    this->duration = blank;
    this->amplitude = blank;
    this->delay = blank;
}

vector< vector<double> >* Input::inputs(double T, double dt, double global_delay) {
    generateSignals(T,dt,global_delay);
    return &signals;
}

void Input::generateSignals(double T, double dt, double global_delay) {
    /** Generate the input signals by
     *  looping over all of the ranges
     *  and producing a unique signal
     *  for each point in the space. */

    double rt; // Used to keep track of the "real time" in ms
    unsigned int steps = (int)(T/dt); // Number of time steps in the signal
    vector<double> signal (steps,0.0);


    switch (this->type) {

        case PURE:

            for (float dur = duration.start; dur <= duration.end; dur += duration.step) {
                for (float amp = amplitude.start; amp <= amplitude.end; amp += amplitude.step) {
                    for (float del = delay.start; del <= delay.end; del += delay.step) {

                        for (unsigned int step = 0; step < steps; ++step) {
                            // Time zero is defined by the global_delay 
                            rt = step * dt - global_delay;

                            if ( (rt-del) >= 0 && (rt-del) <= dur ) {
                                signal[step] = amp;
                            } else {
                                signal[step] = 0;
                            }
                        }

                        // Add signal to the signal collection
                        this->signals.push_back(signal);
                    }    
                }    
            }    
        break;

    } // switch (this->type)
}

string Input::toString() {
    stringstream r;

    // Output input type
    switch(this->type) {
        case PURE:
            r << "Type: Pure\n";
            break;    
        case PULSE:
            r << "Type: Pulse\n";
            break;    
        default:
            r << "Type: Unknown\n";
            break;
    }

    r << "Duration: [" << this->duration.start << "..." << this->duration.end << "] by " << this->duration.step << "\n";
    r << "Amplitude: [" << this->amplitude.start << "..." << this->amplitude.end << "] by " << this->amplitude.step << "\n";
    r << "Delay: [" << this->delay.start << "..." << this->delay.end << "] by " << this->delay.step << "\n";

    return r.str();
}
