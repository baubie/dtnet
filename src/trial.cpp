#include "trial.h"

using namespace std;

bool Trial::load(string filename, string &error)
{
	if (parseXML(filename, error))
    {
        /** Find the unconstrained parameters. **/
        for (vector<Input>::iterator iter=this->inputs.begin(); iter != this->inputs.end(); ++iter) {
            if (iter->duration.start != iter->duration.end) this->unconstrained.push_back("trial."+iter->ID+".duration"); 
            if (iter->amplitude.start != iter->amplitude.end) this->unconstrained.push_back("trial."+iter->ID+".amplitude"); 
            if (iter->delay.start != iter->delay.end) this->unconstrained.push_back("trial."+iter->ID+".delay"); 
        }
        this->filename = filename;
        return true;
    }
    return false;
}

int Trial::count() {
    return this->signals.size();
}

vector<Trial::CombinedInputs>* Trial::inputFactory(double T, double dt, double delay) {
    this->genSignal( this->inputs.begin(), T, dt, delay );
    return &this->signals;
}

void Trial::genSignal(vector<Input>::iterator inputsPos, double T, double dt, double delay) {

    // Get the vector of signals from this input.
    vector<Input::Signal>* thisInput = (*inputsPos).inputs(T, dt, delay);

    /** BASE CASE
     *  We are at the last input
     *  Nothing is in our inputs yet, so add these on
     */
    if (inputsPos+1 == this->inputs.end()) {
        // If we are in the last one, just add these to the input 
        vector< Input::Signal >::iterator iter;
        for (iter = thisInput->begin(); iter < thisInput->end(); ++iter) {
            CombinedInputs ci;
            ci.signals.push_back(*iter); // At the base case, only one input 
            ci.values = iter->values; // Just takes on the values from this signal.
            this->signals.push_back(ci);
        } 
    } 

    
    /** RECURSIVE CASE
     *  First, finish it for everything below
     *  Then create new sets of the input for each of our
     *  local signals.
     */ 
    else {
        genSignal(++inputsPos, T, dt, delay); // Finish up below

        unsigned int steps = (int)(T/dt);

        // Copy of existing inputs
        vector< CombinedInputs > oldSignals (this->signals);

        // We'll be replacing the existing inputs so clear them out
        this->signals.clear();

        vector< Input::Signal >::iterator newIter; // The new input signals
        vector< CombinedInputs >::iterator oldIter; // The old combined inputs 
        vector<double> newValues; // New values

        for (newIter = thisInput->begin(); newIter < thisInput->end(); ++newIter) {
            for (oldIter = oldSignals.begin(); oldIter < oldSignals.end(); ++oldIter) {
                newValues.clear();
                newValues.resize(steps, 0);
                for (unsigned int t = 0; t < steps; ++t) {
                    newValues[t] = newIter->values[t] + oldIter->values[t];
                }
                CombinedInputs ci(*oldIter);
                ci.signals.push_back(*newIter);
                ci.values = newValues;
                this->signals.push_back(ci); // Add on the input
            }
        } 
    }


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
            pElem->QueryValueAttribute("ID", &input.ID);

			pElemParam = hInput.FirstChild( "param" ).Element();
			for ( /***/; pElemParam; pElemParam = pElemParam->NextSiblingElement( "param" )) {

				hParam = pElemParam;
               // Found a Parameter Element With Text
				if (pElemParam->FirstChild()->Type() == TiXmlNode::TEXT) {
                    if (strcmp(pElemParam->Attribute("name"),"duration") == 0) {
                        input.duration.start = (double)atof(pElemParam->FirstChild()->Value());
                        input.duration.end = (double)atof(pElemParam->FirstChild()->Value());
                    }
                    if (strcmp(pElemParam->Attribute("name"),"delay") == 0) {
                        input.delay.start = (double)atof(pElemParam->FirstChild()->Value());
                        input.delay.end = (double)atof(pElemParam->FirstChild()->Value());
                    }
                    if (strcmp(pElemParam->Attribute("name"),"amplitude") == 0) {
                        input.amplitude.start = (double)atof(pElemParam->FirstChild()->Value());
                        input.amplitude.end = (double)atof(pElemParam->FirstChild()->Value());
                    }

               // Found a Parameter Element With A Range Element
				} else if (pElemParam->FirstChild()->Type() == TiXmlNode::ELEMENT) {

					if (strcmp(pElemParam->FirstChild()->Value(), "range") == 0) {
                        if (strcmp(pElemParam->Attribute("name"),"duration") == 0) {
                            hParam.FirstChild().Element()->QueryDoubleAttribute("start", &input.duration.start);
                            hParam.FirstChild().Element()->QueryDoubleAttribute("end", &input.duration.end);
                            hParam.FirstChild().Element()->QueryDoubleAttribute("step", &input.duration.step);
                        }
                        if (strcmp(pElemParam->Attribute("name"),"delay") == 0) {
                            hParam.FirstChild().Element()->QueryDoubleAttribute("start", &input.delay.start);
                            hParam.FirstChild().Element()->QueryDoubleAttribute("end", &input.delay.end);
                            hParam.FirstChild().Element()->QueryDoubleAttribute("step", &input.delay.step);
                        }
                        if (strcmp(pElemParam->Attribute("name"),"amplitude") == 0) {
                            hParam.FirstChild().Element()->QueryDoubleAttribute("start", &input.amplitude.start);
                            hParam.FirstChild().Element()->QueryDoubleAttribute("end", &input.amplitude.end);
                            hParam.FirstChild().Element()->QueryDoubleAttribute("step", &input.amplitude.step);
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


