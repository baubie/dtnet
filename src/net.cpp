
#include "net.h"

using namespace std;


Net::Net() {
    this->initialize();
}


std::vector<Net::ConstrainedNetwork>* Net::networkFactory() {
    if (this->cNetworks.empty()) this->genNetworks();
    return &(this->cNetworks);

}

void Net::genNetworks() {

    // For now just produce a single constrained network.
    // TODO Create a vector of networks for each range value.
    
    ConstrainedNetwork cn;

    map<string, Population>::iterator pIter;

    for (pIter = this->populations.begin(); pIter != this->populations.end(); ++pIter) {
        // TODO Don't just add the first population, use recursion to add all returned populations.
        cn.populations[pIter->first] = pIter->second.populationFactory()->at(0);
    }

    // TODO Don't just create a single weight matrix, use recursion to add in all ranges.
    for (map<string, map<string, Connection<Range> > >::iterator to = this->connections.begin(); to != this->connections.end(); ++to) {
        for (map<string, Connection<Range> >::iterator from = to->second.begin(); from != to->second.end(); ++from) {
            cn.connections[to->first][from->first].weight = (double)from->second.weight;
            cn.connections[to->first][from->first].delay = (double)from->second.delay;
        }
    }

    this->cNetworks.push_back(cn);
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
}

int Net::count_populations() {
    return this->populations.size();
}

string Net::toString() {
    stringstream r;
    
    r << "Network Name: " << this->name << endl;
    r << "Number of Populations: " << this->populations.size() << endl;
    map<string, Population>::iterator iter;
    for ( iter = this->populations.begin(); iter != this->populations.end(); ++iter) {
        r << "== Population ==\n" << (iter->second).toString() << endl;
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
    	pElem->QueryStringAttribute("title", &this->name);
        cout << "Found Network: " << this->name << endl;

        // Load Each Population
        int position = 0;
 		pElem = hRoot.FirstChild( "population" ).Element();
		for( /***/; pElem; pElem = pElem->NextSiblingElement("population")) {

            string pop_name;
            string pop_id;
            char pop_name_tmp[50];
    		int pop_size = 0;
            bool accept_input = false;
            NeuronParams np;
            hPopulation = pElem;        

            if (strcmp(pElem->Attribute("type"), "Poisson") == 0) {
                np = NeuronParams(NeuronParams::POISSON);
            }
            else if (strcmp(pElem->Attribute("type"), "aEIF") == 0) {
                np = NeuronParams(NeuronParams::AEIF);
            }
            pElem->QueryStringAttribute("id", &pop_id);             
            pElem->QueryStringAttribute("title", &pop_name);
            cout << "Found Population: " << pop_name << endl;
            
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

                    for (map<string,Range>::iterator iter = np.vals.begin(); iter != np.vals.end(); ++iter) {
                        if (strcmp(pElemParam->Attribute("name"),(iter->first).c_str()) == 0) {
                            double value = (double)atof(pElemParam->FirstChild()->Value());
                            np.vals[iter->first] = Range(value);
                            double sigma = 0;
                            pElemParam->QueryDoubleAttribute("sigma", &sigma);                    
                            np.sigmas[iter->first] = sigma;
                        }
                    }
                    
               // Found a Parameter Element With A Range Element
				} else if (pElemParam->FirstChild()->Type() == TiXmlNode::ELEMENT) {
				
                }	
			} // Parameter Loop            
            this->populations[pop_id] = Population(pop_name, pop_id, pop_size, accept_input, position, np);
            ++position;
        }

        // Load Each Connection
 		pElem = hRoot.FirstChild( "connection" ).Element();
		for( /***/; pElem; pElem = pElem->NextSiblingElement("connection")) {
            double weight = 0;
            double delay = 1;
            double density = 1;
            double sigma = 0;
            string from, to;
            bool symmetric = false;
			hConnection = pElem;

            pElemParam = hConnection.FirstChild( "param" ).Element();
            for ( /***/; pElemParam; pElemParam = pElemParam->NextSiblingElement( "param" )) {

                hParam = pElemParam;
               // Found a Parameter Element With Text
                if (pElemParam->FirstChild()->Type() == TiXmlNode::TEXT) {
                    if (strcmp(pElemParam->Attribute("name"), "from") == 0) {
                        from = pElemParam->FirstChild()->Value();
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "to") == 0) {
                        to = pElemParam->FirstChild()->Value();
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

            if (weight != 0) {
                this->connections[to][from].weight = Range(weight);
                this->connections[to][from].delay = Range(delay);
            }
		}

        return true;
	} else {
		error = "Unable to find file or XML syntax error.";
		return false;
	}   

}

