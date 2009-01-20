
#include "net.h"

using namespace std;

std::vector<Net::ConstrainedNetwork>* Net::networkFactory() {
    if (this->cNetworks.empty()) this->genNetworks();
    return &(this->cNetworks);

}


void Net::genNetworks() {

    this->genConnections( this->connections.begin(), 
                          this->connections.begin()->second.begin() );

    /** Find the unconstrained connection parameters. **/
    for (map<string, map<string, Connection<Range> > >::iterator iter = this->connections.begin(); iter != this->connections.end(); ++iter) {
        for (map<string, Connection<Range> >::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
            // Contrary to code, we use FROM:TO syntax for the scripting language
            // as it is  more intuitive to users.
            string connection_name = iter2->first + ":" + iter->first; 
            if (iter2->second.weight.size() > 1) this->unconstrained["connection."+connection_name+".weight"] = iter2->second.weight;
            if (iter2->second.delay.size() > 1) this->unconstrained["connection."+connection_name+".delay"] = iter2->second.delay;
            if (iter2->second.density.size() > 1) this->unconstrained["connection."+connection_name+".density"] = iter2->second.density;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // TEMPORARY
    // CURRENTLY ONLY GETS A SINGLE NETWORK
    // EVENTUALLY NEEDS TO CREATE AND PUSH ALL NETWORKS ONTO this->cPopulations
    ////////////////////////////////////////////////////////////////////////////////////////////////
    map<string, Population>::iterator pIter;
    map<string, Population::ConstrainedPopulation> cPop; 
    for (pIter = this->populations.begin(); pIter != this->populations.end(); ++pIter) {
        // TODO Don't just add the first population, use recursion to add all returned populations.
        cPop[pIter->first] = pIter->second.populationFactory()->at(0);
    }
    this->cPopulations.push_back(cPop);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Loop over all networks and all connection profiles to produce all possible constrained networks
    for (vector< map<string, map<string, Connection<double> > > >::iterator cIter = this->cConnections.begin(); cIter != this->cConnections.end(); ++cIter) {
        for (vector< map<string, Population::ConstrainedPopulation> >::iterator pIter = this->cPopulations.begin(); pIter != this->cPopulations.end(); ++pIter) {
            ConstrainedNetwork cn;
            cn.connections = *cIter;
            cn.populations = *pIter;
            this->cNetworks.push_back(cn);
        }
    }


}

void Net::genConnections( map< string, map< string, Connection<Range> > >::iterator to_in,
                          map< string, Connection<Range> >::iterator from_in ) 
{
    /** Recursively generate all possible connections. **/

    map< string, map< string, Connection<Range> > >::iterator to = to_in;
    map< string, Connection<Range> >::iterator from = from_in;

    ///////////////
    // BASE CASE //
    ///////////////
    if (to == --(this->connections.end()) && from == --(to->second.end()) ) {
        this->cConnections.clear();
        for (Range::iterator weight = from->second.weight.begin(); weight != from->second.weight.end(); ++weight) {
            for (Range::iterator delay = from->second.delay.begin(); delay != from->second.delay.end(); ++delay) {
                for (Range::iterator density = from->second.density.begin(); density != from->second.density.end(); ++density) {
                    Connection<double> connection;
                    map<string, map<string, Connection<double> > > network_connection;
                    connection.weight = *weight;
                    connection.delay = *delay;
                    connection.density = *density;
                    network_connection[to->first][from->first] = connection;
                    this->cConnections.push_back( network_connection );
                } 
            } 
        } 
    }

    ////////////////////
    // RECURSIVE CASE //
    ////////////////////
    else {
        // First, finish up for connections below this one.
        map< string, map< string, Connection<Range> > >::iterator new_to = to;
        map< string, Connection<Range> >::iterator new_from = from;
        if (from != --(to->second.end()) ) {
            ++new_from;
        }  else {
            // When we use to in the second parameter below, it has already been incremented.
            ++new_to;
            new_from = new_to->second.begin();
        }
        genConnections( new_to, new_from); 
         
        vector< map<string, map<string, Connection<double> > > > oldConns = this->cConnections;
        this->cConnections.clear();

        // Loop over our existing populations generated above and add on our new connection.
        for (vector< map<string, map<string, Connection<double> > > >::iterator old = oldConns.begin(); old != oldConns.end(); ++old) {
            for (Range::iterator weight = from->second.weight.begin(); weight != from->second.weight.end(); ++weight) {
                for (Range::iterator delay = from->second.delay.begin(); delay != from->second.delay.end(); ++delay) {
                    for (Range::iterator density = from->second.density.begin(); density != from->second.density.end(); ++density) {
                        Connection<double> connection;
                        connection.weight = *weight;
                        connection.delay = *delay;
                        connection.density = *density;
                        (*old)[to->first][from->first] = connection;
                        this->cConnections.push_back( *old );
                    } 
                } 
            } 
        }
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
            Range weight = 0;
            Range delay = 1;
            Range density = 1;
            double sigma = 0;
            double start, end, step;
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
                        weight = Range((double)atof(pElemParam->FirstChild()->Value()));
                        pElem->QueryDoubleAttribute("sigma", &sigma);
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "delay") == 0) {
                        delay = Range((double)atof(pElemParam->FirstChild()->Value()));
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "density") == 0) {
                        density = Range((double)atof(pElemParam->FirstChild()->Value()));
                    } 
                    if (strcmp(pElemParam->Attribute("name"), "symmetric") == 0) {
                        symmetric = (strcmp(pElemParam->FirstChild()->Value(),"true") == 0);
                    } 
				} else if (pElemParam->FirstChild()->Type() == TiXmlNode::ELEMENT) {

					if (strcmp(pElemParam->FirstChild()->Value(), "range") == 0) {
                        start = 0;
                        end = 0;
                        step = 1;
                        hParam.FirstChild().Element()->QueryDoubleAttribute("start", &start);
                        hParam.FirstChild().Element()->QueryDoubleAttribute("end", &end);
                        hParam.FirstChild().Element()->QueryDoubleAttribute("step", &step);
                        if (strcmp(pElemParam->Attribute("name"),"weight") == 0) {
                            weight = Range(start, end, step);
                        }
                        if (strcmp(pElemParam->Attribute("name"),"delay") == 0) {
                            delay = Range(start, end, step);
                        }
                        if (strcmp(pElemParam->Attribute("name"),"density") == 0) {
                            density = Range(start, end, step);
                        }
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

