
#include "net.h"

using namespace std;


Net::Net() : T(50), dt(0.05) {
    this->initialize();
}

Net::Net(double T, double dt) : T(T), dt(dt) {
    this->initialize();
}

void Net::initialize() {
    unsigned int seed = time(NULL);

    struct timespec ts;
    if (0 == clock_gettime(CLOCK_REALTIME, &ts)) {
        seed += (unsigned int)ts.tv_nsec;
    } else {
        cout << "Error: Unable to use clock_gettime();" << endl;
        cout << "ERRNO = " << strerror(errno) << endl;
        throw;
    }
    srand( seed );

    this->steps = (unsigned int)(T/dt);

    this->alphaTauE = 0.7;
    this->alphaTauI = 1.1;
}

int Net::count_populations() {
    return this->populations.size();
}

void Net::createPopulation(string name, string ID, int size, bool accept_input, NeuronParams params) {
    
	populations.push_back(Population(name, ID, size, accept_input, params));
}

int Net::numPopulations() {
    return populations.size();
}

int Net::populationFromID(const string ID) {

	for (unsigned int p=0; p < populations.size(); p++) { // Loop over populations
        if (populations.at(p).ID == ID) return (int)p;
    }
    return Net::NOT_FOUND;
}

bool Net::accept_input(const string ID) {
    int pop = this->populationFromID(ID);
    return (pop != Net::NOT_FOUND && this->populations[pop].accept_input);
}


void Net::finalizePopulations() {	
	const int num = numPopulations();
	this->weights = vector<vector<double> >(num, vector<double> (num, 0.0) );
    this->delays = vector<vector<double> >(num, vector<double> (num, 1.0) );
    this->inputs = vector<vector<double> >(num, vector<double> (steps, 0.0) );
}

void Net::initSimulation() {

    initAlpha((double)1000.0, this->alphaTauE, alphaE);
    initAlpha((double)1000.0, this->alphaTauI, alphaI);

	for (unsigned int p=0; p < populations.size(); ++p) { // Loop over populations
		for (unsigned int n=0; n < populations.at(p).neurons.size(); ++n) { // Loop over neurons
			populations.at(p).neurons.at(n).init(steps);
		}
	}
}

void Net::connectPopulations(int from, int to, double weight, double delay) {
	this->weights[from][to] = weight;
    this->delays[from][to] = delay;
}

void Net::linkinput(vector<double> &input, const string popID) {
    inputs[populationFromID(popID)] = vector<double> (input);
}

void Net::initAlpha(double q, double tau, vector<double> &vals) {

    int alpha_steps = (int)(ALPHA_WIDTH/dt);

    vals.resize(alpha_steps);
    
    double tau2 = pow(tau,2);
    for (int i=0; i < alpha_steps; i++) {
        vals[i] = q*((i*dt) * exp(-(i*dt)/tau)) / tau2;
    }
}

double Net::alpha(double t, vector<double> &spikes, double delay, double weight) {
	double current = 0;
	double spike;
    int step;
    unsigned int numSpikes = spikes.size();

	for (unsigned int s=0; s < numSpikes; s++) {
		spike = t-spikes[s]-delay;

		if (spike > 0) {
            step = (int)(spike/this->dt);
            if (spike < ALPHA_WIDTH) {
                if (weight >= 0) {
                    current += this->alphaE[step];
                } else {
                    current += this->alphaI[step];
                }
            }
		}
	}
	
	return weight*current;
}

void Net::runSimulation() {

	double input;
    double new_input;
	
    ostringstream stm;
		
	for (unsigned int ts=0; ts < steps; ts++) { // Loop over time steps
		for (int p=0; p < (int)populations.size(); p++) { // Loop over populations
			for (int n=0; n < (int)populations.at(p).neurons.size(); n++) { // Loop over neurons	
					
				input = 0.0;
				
				// Find spikes into our population				
				for (int from=0; from < numPopulations(); from++) {
                    new_input = 0;
					if (weights[from][p] != 0) {
						for (unsigned int from_n=0; from_n < populations.at(from).neurons.size(); from_n++){
							new_input += alpha(ts*dt, populations.at(from).neurons.at(from_n).spikes,delays[from][p],weights[from][p]);
						}
						new_input /= (double)populations.at(from).neurons.size();
					}
                    input += new_input;
				}
				
				// Add our input signal in
				input += inputs[p][ts];

				// Update our neuron
				populations.at(p).neurons.at(n).update(input, ts, dt);
			}
		}
	}
}

void Net::saveVoltages(string filename) {
	
	ofstream fout (filename.c_str(), ios::out | ios::binary);
		
	// Output the number of steps as an int
	fout.write((char*)&steps,sizeof(int));
	
	// Output the number of populations
	int pops = populations.size();
	fout.write((char*)&pops, sizeof(int));
	
	// Next write out the timesteps
	double t;
	for (unsigned int ts=0; ts < steps; ts++) {		
		t = ts*dt - 10;
		fout.write((char*)&t, sizeof(double));
	}
	
	for (unsigned int p=0; p < populations.size(); p++) { // Loop over populations
        
		
		// Write out the size of this population
		int popsize = populations.at(p).neurons.size();
		fout.write((char*)&popsize, sizeof(int));
				
		// Write out the name
		const char* name;
		name = populations.at(p).name.c_str();
		int namesize = populations.at(p).name.size();
		fout.write((char*)&namesize, sizeof(int));
		fout.write(const_cast<char*>(name), sizeof(char)*namesize);
		for (unsigned int n=0; n < populations.at(p).neurons.size(); n++) { // Loop over neurons
            fout.write((char*)&populations.at(p).neurons.at(n).voltage[0], sizeof(double)*steps);
			// Write out the spike times
			int numspikes = populations.at(p).neurons.at(n).spikes.size();
			fout.write((char*)&numspikes, sizeof(int));
			fout.write((char*)&populations.at(p).neurons.at(n).spikes[0],sizeof(double)*numspikes);
		}
	}
	
	fout.close();
}

void Net::geninput(vector<double>* input, double duration, double mu, double delay) {
   /* Create an input signal
	* fills double input with values in [0,inf]
	* Where 0 is no firing and 1 is firing at mu
	* Therefore 2 is firing at 2*mu, etc.
	*/
	
	double maximum = 1000;
	
	double initial_spike_length = 1.0;
	double half_initial_length = 1.0;

	double initial_spike_height = (double)maximum/(double)mu;
	double half_initial_spike_height = 1.0 + (initial_spike_height-1.0)/2.0;

	double inputMu;	
	double ramp_length = 0.2;
	unsigned int step;
	
	// Setup baseline
    for (double t=0; t < delay; t += this->dt) {
        input->push_back(0.0);
    }
	for (double t=delay; t<delay+duration; t += this->dt) {		
		step = (unsigned int)(t / this->dt);

		inputMu = 1.0;

        // Add in that initial spiking increase
		if (t-delay < initial_spike_length) {
			if (mu < 100) {
				inputMu = 1;
			} else if (mu > 500) {
				inputMu = initial_spike_height;     
			} else {
				inputMu = 1.0+(initial_spike_height-1.0)*sqrt((mu-100.0)/400.0);
			}
		}
		else if (t-delay <= initial_spike_length + half_initial_length) {
			if (mu < 100) {
				inputMu = 1;
			} else if (mu > 500) {
				inputMu = half_initial_spike_height;     
			} else {
				inputMu = 1.0+(half_initial_spike_height-1.0)*sqrt((mu-100.0)/400.0);
			}
		}

        // Add in the ramping attenuation.
		if (t - delay < ramp_length) {
			inputMu *= (t-delay)/ramp_length;
		}
		if (duration-(t-delay) < ramp_length) {
			inputMu *= (duration-(t-delay))/ramp_length;
		}

		input->push_back(inputMu);
	}
    for (double t=delay+duration; t < this->T; t += this->dt) {
        input->push_back(0.0);
    }
}

string Net::toString() {
    stringstream r;
    
    r << "Network Name: " << this->name << endl;
    r << "Number of Populations: " << this->populations.size() << endl;
    vector<Population>::iterator iter;
    for ( iter = this->populations.begin(); iter != this->populations.end(); ++iter) {
        r << "== Population ==\n" << (*iter).toString() << endl;
    }
    return r.str();
}

bool Net::load(std::string filename, std::string &error)
{
    if (parseXML(filename, error))
    {
        this->filename = filename;
        return true;
    }
    return false;
}


bool Net::parseXML(string filename, string &error)
{
	TiXmlDocument doc(filename);

	if (doc.LoadFile()) {

		stringstream name;

		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem;
		TiXmlElement* pElemParam;
		TiXmlHandle hRoot(0);
		TiXmlHandle hConnection(0);
		TiXmlHandle hPopulation(0);
		TiXmlHandle hParam(0);

        // Find Root
		pElem=hDoc.FirstChildElement().Element();
		if (!pElem) {
			error = "Invalid root.";
			return false;
		}
		hRoot = pElem;
	
        // Load Network Attributes 
    	pElem->QueryValueAttribute("name", &this->name);

        // Load Each Population
 		pElem = hRoot.FirstChild( "population" ).Element();
		for( /***/; pElem; pElem = pElem->NextSiblingElement("population")) {

            string pop_name;
            string pop_id;
    		int pop_size = 0;
            bool accept_input = false;
            NeuronParams np = Neuron::defaultParams();
            hPopulation = pElem;        

            if (strcmp(pElem->Attribute("type"), "Poisson") == 0) {
                np.type = NeuronParams::POISSON;
            }
            if (strcmp(pElem->Attribute("type"), "aEIF") == 0) {
                np.type = NeuronParams::AEIF;
            }
            pElem->QueryValueAttribute("id", &pop_id);             
            pElem->QueryValueAttribute("name", &pop_name);             
            
			pElemParam = hPopulation.FirstChild( "param" ).Element();
			for ( /***/; pElemParam; pElemParam = pElemParam->NextSiblingElement( "param" )) {

				hParam = pElemParam;
               // Found a Parameter Element With Text
				if (pElemParam->FirstChild()->Type() == TiXmlNode::TEXT) {

                    if (strcmp(pElemParam->Attribute("name"),"accept_input") == 0) {
                        accept_input = (strcmp(pElemParam->FirstChild()->Value(), "true") == 0);
                    }

                    if (strcmp(pElemParam->Attribute("name"),"size") == 0) {
                        pop_size = (int)atoi(pElemParam->FirstChild()->Value());
                    }
         
                    if (strcmp(pElemParam->Attribute("name"),"VT") == 0) {
                        np.aEIF.VT = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jVT);                    
                    }
                    
                    if (strcmp(pElemParam->Attribute("name"),"C") == 0) {
                        np.aEIF.C = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jC);                    
                    }
                    
                    if (strcmp(pElemParam->Attribute("name"),"hypTau") == 0) {
                        np.aEIF.hypTau = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jhypTau);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"alpha_q") == 0) {
                        np.aEIF.alpha_q = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jalpha_q);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"gL") == 0) {
                        np.aEIF.gL = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jgL);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"EL") == 0) {
                        np.aEIF.EL = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jEL);                    
                    }
                                        
                    if (strcmp(pElemParam->Attribute("name"),"tauw") == 0) {
                        np.aEIF.tauw = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jtauw);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"a") == 0) {
                        np.aEIF.a = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.ja);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"b") == 0) {
                        np.aEIF.b = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jb);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"deltaT") == 0) {
                        np.aEIF.deltaT = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jdeltaT);                    
                    }

                    if (strcmp(pElemParam->Attribute("name"),"VR") == 0) {
                        np.aEIF.VR = (double)atof(pElemParam->FirstChild()->Value());
                		pElem->QueryDoubleAttribute("sigma", &np.aEIF.jVR);                    
                    }
                    
               // Found a Parameter Element With A Range Element
				} else if (pElemParam->FirstChild()->Type() == TiXmlNode::ELEMENT) {
				
                }	
			} // Parameter Loop            
            createPopulation(pop_name, pop_id, pop_size, accept_input, np);
        }

        this->finalizePopulations();
        // Load Each Connection
 		pElem = hRoot.FirstChild( "connection" ).Element();
		for( /***/; pElem; pElem = pElem->NextSiblingElement("connection")) {
            int from = -1;
            int to = -1;
            double weight = 0;
            double delay = 1;
            double density = 1;
            double sigma = 0;
            bool symmetric = false;
			hConnection = pElem;

            pElemParam = hConnection.FirstChild( "param" ).Element();
            for ( /***/; pElemParam; pElemParam = pElemParam->NextSiblingElement( "param" )) {

                hParam = pElemParam;
               // Found a Parameter Element With Text
                if (pElemParam->FirstChild()->Type() == TiXmlNode::TEXT) {
                    if (strcmp(pElemParam->Attribute("name"), "from") == 0) {
                        from = this->populationFromID(pElemParam->FirstChild()->Value());
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "to") == 0) {
                        to = this->populationFromID(pElemParam->FirstChild()->Value());
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "weight") == 0) {
                        weight = (double)atof(pElemParam->FirstChild()->Value());
                        pElem->QueryDoubleAttribute("sigma", &sigma);
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "delay") == 0) {
                        delay = (double)atof(pElemParam->FirstChild()->Value());
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "density") == 0) {
                        density = (double)atof(pElemParam->FirstChild()->Value());
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "symmetric") == 0) {
                        symmetric = (strcmp(pElemParam->FirstChild()->Value(),"true") == 0);
                    } 
                } 
            }
            if (from != -1 && to != -1) connectPopulations(from, to, weight, delay);
		}

        return true;
	} else {
		error = "Unable to find file or XML syntax error.";
		return false;
	}   

}

