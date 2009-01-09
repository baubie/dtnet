
#include "input.h"

using namespace std;

Input::Input() {
    Range blank;
    this->duration = blank;
    this->amplitude = blank;
    this->delay = blank;
}

vector<Input::Signal>* Input::inputs(double T, double dt, double global_delay) {
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
    vector<double> values (steps,0.0);

    switch (this->type) {

        case PURE:
            for (double dur = duration.start; dur <= duration.end; dur += duration.step) {
                for (double amp = amplitude.start; amp <= amplitude.end; amp += amplitude.step) {
                    for (double del = delay.start; del <= delay.end; del += delay.step) {

                        for (unsigned int step = 0; step < steps; ++step) {
                            // Time zero is defined by the global_delay 
                            rt = step * dt - global_delay;

                            if ( (rt-del) >= 0 && (rt-del) <= dur ) {
                                values[step] = amp;
                            } else {
                                values[step] = 0;
                            }
                        }

                        // Add signal to the signal collection
                        Signal sig; 
                        sig.values = values;
                        sig.duration = dur;
                        sig.amplitude = amp;
                        sig.delay = del;
                        sig.ID = this->ID;
                        this->signals.push_back(sig);
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

    r << "Duration: ";
    if (this->duration.start != this->duration.end) r << "[" << this->duration.start << "..." << this->duration.end << "] by " << this->duration.step << "\n";
    else r << this->duration.start << "\n";

    r << "Amplitude: ";
    if (this->amplitude.start != this->amplitude.end) r << "[" << this->amplitude.start << "..." << this->amplitude.end << "] by " << this->amplitude.step << "\n";
    else r << this->amplitude.start << "\n";

    r << "Delay: ";
    if (this->delay.start != this->delay.end) r << "[" << this->delay.start << "..." << this->delay.end << "] by " << this->delay.step << "\n";
    else r << this->delay.start << "\n";

    return r.str();
}
