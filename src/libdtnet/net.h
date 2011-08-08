
#ifndef NET_H
#define NET_H

#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "population.h"
#include "neuron.h"
#include "lib/tinyxml/tinyxml.h"
#include "serialization.h"
#include "range.h"
#include "debug.h"

class Net {
public:
    bool verbose;
    std::string name; // A name for this network

    Net();

    void linkinput(std::vector<double> &input);
    bool load(std::string filename, std::string &error);
    int count_populations();
    std::string toString();
    void initSimulation(double T, double dt, double delay);
    bool isReady();
    
    template<class T>
    struct Connection {
        T delay;
        T weight;
        T density;
        T sigma;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & delay;
            ar & weight;
            ar & density;
            ar & sigma;
        }        
    };

    static bool pop_order_sort(Population::ConstrainedPopulation* a, Population::ConstrainedPopulation* b) {
        return (a->position < b->position);
    }

    struct ConstrainedNetwork {
        std::map<std::string, Population::ConstrainedPopulation> populations;
        std::map< std::string, std::map< std::string, Connection<double> > > connections;

        std::vector<Population::ConstrainedPopulation*> popvector() {
            std::vector<Population::ConstrainedPopulation*> r;
            for (std::map<std::string, Population::ConstrainedPopulation>::iterator i = this->populations.begin(); i != this->populations.end(); ++i) {
                r.push_back(&(i->second));
            }
            std::sort(r.begin(), r.end(), Net::pop_order_sort);
            return r;
        }

        std::map<double, std::vector<double> > alphaVals;

        double alpha(double t, std::list<Neuron*> neurons, double tau, double delay, double globalDelay, double dt) {

            static int ALPHA_WIDTH = 20;
            static int alpha_steps = (int) (ALPHA_WIDTH / dt);
            static double q = 1000.0;

            if (alphaVals.find(tau) == alphaVals.end()) {
                alphaVals[tau] = std::vector<double>(alpha_steps, 0);
                double tau2 = pow(tau, 2);
                for (int i = 0; i < alpha_steps; i++) {
                    alphaVals[tau][i] = q * ((i * dt) * exp(-(i * dt) / tau)) / tau2;
                }
            }

            double current = 0;
            double spike;
            int step;


            for (std::list<Neuron*>::iterator n = neurons.begin(); n != neurons.end(); ++n) {
                // Loop over the spikes from the most recent the the oldest
                for (std::vector<double>::reverse_iterator s = (*n)->spikes.rbegin(); s != (*n)->spikes.rend(); ++s) {
                    // delay is the axonal delay and this->delay is the global time zero delay
                    spike = t - *s - delay - globalDelay;
                    if (spike > 0) {
                        step = (int) (spike / dt);
                        if (spike < ALPHA_WIDTH) {
                            current += alphaVals[tau][step];
                        } else {
                            break;
                        }
                    }
                }
            }
            return current;
        }


        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & populations;
            ar & connections;
        }

    };

    std::vector<ConstrainedNetwork>* networkFactory();

    bool accept_input(const std::string popID);
    static const int NOT_FOUND = -999;

    std::map< std::string, Range > unconstrained;

    std::map<std::string, Population> populations;
    std::map< std::string, std::map< std::string, Connection<Range> > > connections;


private:

    void genConnections(std::map< std::string, std::map< std::string, Connection<Range> > >::iterator to,
            std::map< std::string, Connection<Range> >::iterator from);

    void genPopulations(std::vector< std::vector<Population::ConstrainedPopulation>* >::iterator pop_in);

    std::vector< std::vector<Population::ConstrainedPopulation>* > populationCollections;

    std::vector< std::map< std::string, std::map< std::string, Connection<double> > > > cConnections; /**< Constrained connections. **/
    std::vector< std::map< std::string, Population::ConstrainedPopulation > > cPopulations; /**< Constrained collections of populations. **/
    std::vector<ConstrainedNetwork> cNetworks; /**< Completed, constrained networks taking all combinations of cPopulations and cConnections. **/

    void genNetworks();
    std::string filename;
    int numPopulations();
    bool parseXML(std::string filename, std::string &error);
    void initialize();
    
    bool ready;
};
#endif

