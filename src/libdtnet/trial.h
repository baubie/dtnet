
#ifndef TRIAL_H
#define TRIAL_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "input.h"
#include "range.h"
#include "lib/tinyxml/tinyxml.h"
#include "serialization.h"
#include "debug.h"

/** Load and generate the input signal vectors from an XML file. */
class Trial {
public:
    bool load(std::string filename, std::string &error); /**< Load a trial from an XML file. */

    Trial();

    int count(); /**< Provide the number of input signals generated in this trial. */
    std::string toString(); /**< Provide a string representation of this trial. */

    struct ConstrainedTrial {
        std::vector< Input::Signal > signals;
        std::vector<double> values; /**< Combined values. **/

        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & signals;
            ar & values;
        }
    };

    std::map< std::string, Range > unconstrained; /*<< Collection of unconstrained IDs. */
    std::vector<ConstrainedTrial>* inputFactory(double T, double dt, double delay);

    bool isReady();

private:
    std::vector<Input> inputs; /**< Collection of input signal definitions to generate vectors from. */
    std::vector< ConstrainedTrial > cTrials; /**< Collection of constrained signals. */

    bool parseXML(std::string filename, std::string &error); /**< Parse the XML file. */
    void genSignal(std::vector<Input>::iterator inputsPos, double T, double dt, double delay); /**< Generate input signals from the collection of input definitions. */
    std::string filename; /**< XML from which this Trial was read from. */
    
    bool ready;
};
#endif
