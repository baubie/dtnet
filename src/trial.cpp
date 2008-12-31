#include "trial.h"

using namespace std;

Trial::Trial(double T, double dt, double delay) : T(T), dt(dt), delay(delay) {}

bool Trial::load(string filename, string &error)
{
	if (parseXML(filename, error))
    {
        genSignal(this->inputs.begin());
        genTimeSeries();
        this->filename = filename;
        return true;
    }
    return false;
}

int Trial::count() {
    return this->inputSignals.size();
}

vector<vector<double> >* Trial::signals() {
    return &(this->inputSignals);
}

void Trial::genTimeSeries() {
        unsigned int steps = (int)(T/dt);
        timesteps.resize(steps);
        for (unsigned int i = 0; i < steps; ++i) {
            timesteps[i] = i * dt - delay;
        }
}

void Trial::genSignal(vector<Input>::iterator inputsPos) {

    vector< vector<double> >* thisInput = (*inputsPos).inputs(this->T, this->dt, this->delay);

    /** BASE CASE
     *  We are at the last input
     *  Nothing is in our inputs yet, so add these on
     */
    if (inputsPos+1 == this->inputs.end()) {
        // If we are in the last one, just add these to the input 
        vector< vector<double> >::iterator iter;
        for (iter = thisInput->begin(); iter < thisInput->end(); ++iter) {
            this->inputSignals.push_back(*iter); // Add on the input
        } 
    } 

    
    /** RECURSIVE CASE
     *  First, finish it for everything below
     *  Then create new sets of the input for each of our
     *  local signals.
     */ 
    else {
        genSignal(++inputsPos); // Finish up below

        unsigned int steps = (int)(T/dt);

        // Copy of existing inputs
        vector< vector<double> > oldInputs (this->inputSignals);

        // We'll be replacing the existing inputs so clear them out
        this->inputSignals.clear();

        vector< vector<double> >::iterator newIter;
        vector< vector<double> >::iterator oldIter; 
        vector<double> newSignal;
        for (newIter = thisInput->begin(); newIter < thisInput->end(); ++newIter) {
            for (oldIter = oldInputs.begin(); oldIter < oldInputs.end(); ++oldIter) {
                newSignal.clear();
                newSignal.resize(steps, 0);
                for (unsigned int t = 0; t < steps; ++t) {
                    newSignal[t] = newIter->at(t) + oldIter->at(t);
                }
                this->inputSignals.push_back(newSignal); // Add on the input
            }
        } 
    }


}

vector<double>* Trial::timeSteps() {
    return &(this->timesteps);
}

string Trial::toString() {
    string r;
    vector<Input>::iterator iter;
    for ( iter = this->inputs.begin(); iter != this->inputs.end(); ++iter) {
        r += "== Input ==\n" + (*iter).toString() + "\n";
    }
    return r;
}


bool Trial::parseXML(string filename, string &error) {
	TiXmlDocument doc(filename);

	if (doc.LoadFile()) {

		stringstream name;

		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem;
		TiXmlElement* pElemParam;
		TiXmlHandle hRoot(0);
		TiXmlHandle hInput(0);
		TiXmlHandle hParam(0);

        // Find Root
		pElem=hDoc.FirstChildElement().Element();
		if (!pElem) {
			error = "Invalid root.";
			return false;
		}
		hRoot = pElem;

        // Load the trial information
		pElem->QueryDoubleAttribute("T", &this->T); 
		pElem->QueryDoubleAttribute("dt", &this->dt); 
		pElem->QueryDoubleAttribute("delay", &this->delay); 
		pElem->QueryValueAttribute("name", &this->name);
		pElem->QueryValueAttribute("ID", &this->ID);

		// Load Each Input
		pElem = hRoot.FirstChild( "input" ).Element();
		for( /***/; pElem; pElem = pElem->NextSiblingElement("input")) {

            Input input;
			hInput = pElem;


            if (strcmp(pElem->Attribute("type"), "pure") == 0) {
                input.type = Input::PURE;
            }
            if (strcmp(pElem->Attribute("type"), "pulse") == 0) {
                input.type = Input::PULSE;
            }

			pElemParam = hInput.FirstChild( "param" ).Element();
			for ( /***/; pElemParam; pElemParam = pElemParam->NextSiblingElement( "param" )) {

				hParam = pElemParam;
               // Found a Parameter Element With Text
				if (pElemParam->FirstChild()->Type() == TiXmlNode::TEXT) {
                    if (strcmp(pElemParam->Attribute("name"),"duration") == 0) {
                        input.duration.start = (float)atof(pElemParam->FirstChild()->Value());
                        input.duration.end = (float)atof(pElemParam->FirstChild()->Value());
                    }
                    if (strcmp(pElemParam->Attribute("name"),"delay") == 0) {
                        input.delay.start = (float)atof(pElemParam->FirstChild()->Value());
                        input.delay.end = (float)atof(pElemParam->FirstChild()->Value());
                    }
                    if (strcmp(pElemParam->Attribute("name"),"amplitude") == 0) {
                        input.amplitude.start = (float)atof(pElemParam->FirstChild()->Value());
                        input.amplitude.end = (float)atof(pElemParam->FirstChild()->Value());
                    }

               // Found a Parameter Element With A Range Element
				} else if (pElemParam->FirstChild()->Type() == TiXmlNode::ELEMENT) {

					if (strcmp(pElemParam->FirstChild()->Value(), "range") == 0) {
                        if (strcmp(pElemParam->Attribute("name"),"duration") == 0) {
                            hParam.FirstChild().Element()->QueryFloatAttribute("start", &input.duration.start);
                            hParam.FirstChild().Element()->QueryFloatAttribute("end", &input.duration.end);
                            hParam.FirstChild().Element()->QueryFloatAttribute("step", &input.duration.step);
                        }
                        if (strcmp(pElemParam->Attribute("name"),"delay") == 0) {
                            hParam.FirstChild().Element()->QueryFloatAttribute("start", &input.delay.start);
                            hParam.FirstChild().Element()->QueryFloatAttribute("end", &input.delay.end);
                            hParam.FirstChild().Element()->QueryFloatAttribute("step", &input.delay.step);
                        }
                        if (strcmp(pElemParam->Attribute("name"),"amplitude") == 0) {
                            hParam.FirstChild().Element()->QueryFloatAttribute("start", &input.amplitude.start);
                            hParam.FirstChild().Element()->QueryFloatAttribute("end", &input.amplitude.end);
                            hParam.FirstChild().Element()->QueryFloatAttribute("step", &input.amplitude.step);
                        }
					}
				}
			} // Parameter Loop
            this->inputs.push_back(input);
		} // Input Loop 

		return true;
	} else {
		error = "Unable to find file or XML syntax error.";
		return false;
	}
	
}


