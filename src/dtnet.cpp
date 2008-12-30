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
    GLE::gv = false;
    int procs = 8;

    string script;
    string prompt = "> ";
    char* home = getenv("HOME");
    string history(home);
    history.append("/.dtnet_history");

    /* Parse Command Line Arguments */
    int result = parseargs(argc, argv, dtlang::verbose, script, procs, GLE::gv);
    if (result != 0) return result;

    /* Display A Welcome Message */
    if (dtlang::verbose) cout << endl << "Welcome to the Parallel Network Simulator 2.0" << endl;
    if (dtlang::verbose) cout << "Written by Brandon Aubie <aubiebn@mcmaster.ca>" << endl;
    if (dtlang::verbose) cout << "Compiled " << __DATE__ << " " << __TIME__ << endl;
	
    /* Setup Threads For Parallel Simulations */
    if (dtlang::verbose) cout << "...Pooling " << procs << " threads for simulations";
    boost::threadpool::pool tp(procs);
    if (dtlang::verbose) cout << "\t[OK]" << endl;

    /* Main Input Loop */
    if (dtlang::verbose) cout << "Initialization Complete" << endl;
    if (dtlang::verbose) cout << "Simulation is Empty" << endl << endl;

    SReadline   Reader(history, 32);
    dtlang::initialize(); 

    // Register the built in functions for autocomplete
    vector<string> Completers;
    map<string, dtlang::function_def>::iterator iter;
    for (iter = dtlang::functions.begin(); iter != dtlang::functions.end(); ++iter) {
        Completers.push_back( iter->first );
    }
    Reader.RegisterCompletions( Completers );

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





