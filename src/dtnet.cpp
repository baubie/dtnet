/*	Parallel Network Simulator
 *  Written by Brandon Aubie
 *  aubiebn@mcmaster.ca
 */

#include "dtnet.h"

using namespace std;
using namespace swift;


int main(int argc, char* argv[]) {

    /* Default Values */
    dtlang::verbose = false;
    int procs = 1;

    string script;
    string prompt = "> ";
    char* home = getenv("HOME");
    string history(home);
    history.append("/.dtnet_history");

    /* Parse Command Line Arguments */
    int result = parseargs(argc, argv, dtlang::verbose, script, procs, GLE::viewer);
    if (result != 0) return result;

    /* Display A Welcome Message */
    if (dtlang::verbose) cout << endl << "Welcome to the Parallel Network Simulator 2.0" << endl;
    if (dtlang::verbose) cout << "Written by Brandon Aubie <aubiebn@mcmaster.ca>" << endl;
    if (dtlang::verbose) cout << "Compiled " << __DATE__ << " " << __TIME__ << endl;
#ifdef SERIALIZE_TEXT
    if (dtlang::verbose) cout << "Archive format is text." << endl;
#else
    if (dtlang::verbose) cout << "Archive format is binary." << endl;
#endif

    /* Setup Threads For Parallel Simulations */
    if (dtlang::verbose) cout << "...Pooling " << procs << " threads for simulations";
    boost::threadpool::pool tp(procs+1); // Pool an additional thread for the progress meter
    if (dtlang::verbose) cout << "\t[OK]" << endl;

    /* Main Input Loop */
    if (dtlang::verbose) cout << "Initialization Complete" << endl;
    if (dtlang::verbose) cout << "Simulation is Empty" << endl << endl;

    SReadline   Reader(history, 32);
    dtlang::initialize();

    bool EndOfInput = false;
    // Run an external script from the command line
    if (script != "") {
        dtlang::parse("external(\"" + script + "\")", tp, EndOfInput);
    }

    string input;
    stringstream ssprompt;
    ssprompt << VT_set_colors(VT_BLUE, VT_DEFAULT) << "dtnet" << VT_default_attributes << "> ";
    while (!EndOfInput) {
        input = Reader.GetLine(ssprompt.str(), EndOfInput);
        if (dtlang::parse(input, tp, EndOfInput)) {
            Reader.SaveHistory(history);
        }
    }
    tp.wait();
    return 0;
}





