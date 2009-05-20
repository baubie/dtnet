/* Unit test for dtnet
 * Simply application for use in testing
 * and profiling
 */

#include "../libdtnet/libdtnet.h"
#include <iostream>
#include <string>

#define OK(msg) std::cout << "[OK] " << msg << std::endl;
#define BAD(msg,err) std::cout << "[BAD] " << msg << " Error: " << err << std::endl;

int main(int argc, char* argv[]) {

    int threads = 8;

    dtnet::initialize();
    OK("dtnet::initialize()");

    dtnet::set_threads(threads);
    OK("dtnet::set_threads()");

    Net *net = new Net();
    Trial *trial = new Trial();
    Results *results = new Results();

    std::string error;
    if (net->load("net.xml", error)) { OK("net->load()"); }
    else { BAD("net->load()",error); return 1; }

    if (trial->load("input.xml", error)) { OK("input->load()"); }
    else { BAD("input->load()",error); return 1; }

    Simulation sim(*net, *trial);
    dtnet::set("T", 100);
    dtnet::set("dt", 0.05);
    dtnet::run(*results, 
               sim, 
               "",
               10,        // Run 10 trials
               5,         // Add 10 ms buffer
               true       // Save voltage
               );
    OK("dtnet::run()")

    delete net;
    delete trial;
    delete results;

    return 0;
}
