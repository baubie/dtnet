
#include "net.h"

Net::Net() {

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
}

int Net::createPopulation(string name, int size, NeuronParams params) {
    
	populations.push_back(Population(name, size, params));
	return populations.size()-1;
}

int Net::numPopulations() {
    return populations.size();
}

void Net::finalizePopulations() {	
	
	const int num = numPopulations();
	
    vector<vector<double> > weights(num, vector<double> (num, 0.0) );
    this->weights = weights;

    vector<vector<double> > delays(num, vector<double> (num, 1.0) );
    this->delays = delays;
	
    vector<vector<double> > inputs(num, vector<double> (steps, 0.0) );
    this->inputs = inputs;
}

void Net::initSimulation() {

    initAlpha((float)1000.0, this->alphaTauE, alphaE);
    initAlpha((float)1000.0, this->alphaTauI, alphaI);

	for (unsigned int p=0; p < populations.size(); p++) { // Loop over populations
		for (unsigned int n=0; n < populations.at(p).neurons.size(); n++) { // Loop over neurons
			populations.at(p).neurons.at(n).init(steps);
		}
	}
}

void Net::connectPopulations(int from, int to, double weight, double delay) {
	this->weights[from][to] = weight;
    this->delays[from][to] = delay;
}

void Net::connectInput(int to, vector<double> input) {
    inputs[to] = vector<double> (input);
}

void Net::initAlpha(float q, float tau, vector<double> &vals) {

    int alpha_steps = (int)(ALPHA_WIDTH/dt);

    vals.resize(alpha_steps);
    
    double tau2 = pow(tau,2);
    for (int i=0; i < alpha_steps; i++) {
        vals[i] = q*((i*dt) * exp(-(i*dt)/tau)) / tau2;
    }
}

double Net::alpha(float t, vector<float> &spikes, double delay, double weight) {
	double current = 0;
	float spike;
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
	float t;
	for (unsigned int ts=0; ts < steps; ts++) {		
		t = ts*dt - 10;
		fout.write((char*)&t, sizeof(float));
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
            fout.write((char*)&populations.at(p).neurons.at(n).voltage[0], sizeof(float)*steps);
			// Write out the spike times
			int numspikes = populations.at(p).neurons.at(n).spikes.size();
			fout.write((char*)&numspikes, sizeof(int));
			fout.write((char*)&populations.at(p).neurons.at(n).spikes[0],sizeof(float)*numspikes);
		}
	}
	
	fout.close();
}

void Net::geninput(vector<double>* input, float duration, float mu, double delay) {
   /* Create an input signal
	* fills float input with values in [0,inf]
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




void Net::loadNetwork(string filename) 
{
    if (verbose) cout << "...Loading Network From " << filename << endl;

    /*

	using namespace libconfig;	
	
	Config cfg;
	cfg.readFile(filename.c_str());

    int T = 70;    
    cfg.lookupValue("simulation.T", T);
    this->T = T;

    float dt = 0.05;
    cfg.lookupValue("simulation.dt", dt);
    this->dt = dt;

    this->steps = (unsigned int)(T/dt);

    float alphaTauE = 0.7;
    float alphaTauI = 1.1;
    cfg.lookupValue("simulation.alphaTauE", alphaTauE);
    cfg.lookupValue("simulation.alphaTauI", alphaTauI);
    this->alphaTauE = alphaTauE;
    this->alphaTauI = alphaTauI;

	cfg.lookup("populations.names");
	Setting& popCodes = cfg.lookup("populations.names");
    int popNumber = popCodes.getLength();
	int popSize[popNumber];
	int population[popNumber];
	int extinput[popNumber];
    map<string, int> popID;
	vector<string> popNames(popNumber);
	
	// Setup neuron parameters
	for (int i = 0; i < popNumber; i++) {		
		// Setup config file base string
		const char* name = popCodes[i];	
		string mu = "";	
		string base = "populations.";
		base += name;
		base += ".";
		
		// Default values
		NeuronParams np = Neuron::defaultParams();
		bool usePoisson = false;
		popSize[i] = 1;
		extinput[i] = 0;
		
		// Load population size
		cfg.lookupValue(base+"size", popSize[i]);
		
		// Load population name
		cfg.lookupValue(base+"name", popNames[i]);
		
		// Load population type
		cfg.lookupValue(base+"Poisson", usePoisson);
		if (usePoisson) np.type = NeuronParams::POISSON;
		
		// Load population parameters
		cfg.lookupValue(base+"C", np.C);
		cfg.lookupValue(base+"gL", np.gL);
		cfg.lookupValue(base+"EL", np.EL);
		cfg.lookupValue(base+"VT", np.VT);
		cfg.lookupValue(base+"deltaT", np.deltaT);
		cfg.lookupValue(base+"tauw", np.tauw);
		cfg.lookupValue(base+"a", np.a);
		cfg.lookupValue(base+"b", np.b);
		cfg.lookupValue(base+"VR", np.VR);
		cfg.lookupValue(base+"jC", np.jC);
		cfg.lookupValue(base+"jgL", np.jgL);
		cfg.lookupValue(base+"jEL", np.jEL);
		cfg.lookupValue(base+"jVT", np.jVT);
		cfg.lookupValue(base+"jdeltaT", np.jdeltaT);
		cfg.lookupValue(base+"jtauw", np.jtauw);
		cfg.lookupValue(base+"ja", np.ja);
		cfg.lookupValue(base+"jb", np.jb);
		cfg.lookupValue(base+"jVR", np.jVR);
		population[i] = createPopulation(popNames[i], popSize[i], np);
        popID[popCodes[i]] = population[i];
	}	
	finalizePopulations();

    // Connect the populations
	Setting& network = cfg.lookup("network");
    for (int i=0; i < network.getLength(); i++)
    {
        if (popID.find((const char *)network[i][0]) == popID.end()) {
            cout << "Error: " << (const char *)network[i][0] << " is in network table but is not defined." << endl;
            throw;
        }
        if (popID.find((const char *)network[i][1]) == popID.end()) {
            cout << "Error: " << (const char *)network[i][1] << " is in network table but is not defined." << endl;
            throw;
        }
        //connectPopulations(popID[(const char *)network[i][0]], popID[(const char *)network[i][1]], (double)network[i][2], (double)network[i][3]);
    }
    */
}
