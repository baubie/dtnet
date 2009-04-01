
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
    double ramp_up = 0.2;
    double ramp_down = 0.2;

    switch (this->type) {

        case PURE:
            for (vector<double>::iterator dur = duration.values.begin(); dur != duration.values.end(); ++dur) {
                for (vector<double>::iterator amp = amplitude.values.begin(); amp != amplitude.values.end(); ++amp) {
                    for (vector<double>::iterator del = delay.values.begin(); del != delay.values.end(); ++del) {

                        for (unsigned int step = 0; step < steps; ++step) {
                            // Time zero is defined by the global_delay 
                            rt = step * dt - global_delay;

                            if ( (rt-*del) >= 0 && (rt-*del) <= *dur ) {
                                values[step] = *amp;

                                // Add in the up and down ramps to the signal
                                if ((rt-*del) <= ramp_up && ramp_up > 0) {
                                    values[step] *= (rt-*del) / ramp_up;
                                }
                                if ((rt-*del) >= *dur-ramp_down && ramp_down > 0) {
                                    values[step] *= (*dur - (rt-*del)) / ramp_down;
                                }

                            } else {
                                values[step] = 0;
                            }
                        }

                        // Add signal to the signal collection
                        Signal sig; 
                        sig.values = values;
                        sig.duration = *dur;
                        sig.amplitude = *amp;
                        sig.delay = *del;
                        sig.ID = this->ID;
                        this->signals.push_back(sig);
                    }    
                }    
            }    

            // Figure out the unconstrained parameters
            if (duration.size() > 1) this->unconstrained["trial."+this->ID+".duration"] = duration;
            if (amplitude.size() > 1) this->unconstrained["trial."+this->ID+".amplitude"] = amplitude;
            if (delay.size() > 1) this->unconstrained["trial."+this->ID+".delay"] = delay;
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

    r << "Total Signals: " << (duration.size() * amplitude.size() * delay.size()) << "\n";
    return r.str();
}
