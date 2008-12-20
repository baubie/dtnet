/*	Parallel Network Simulator
 *  Written by Brandon Aubie
 *  aubiebn@mcmaster.ca
 */

#include "dtnet.h"

using namespace std;
using namespace swift;

int main(int argc, char* argv[]) {
	
    /* Default Values */
    bool verbose = false;
    int procs = 8;

    string script;
    string prompt = "> ";
    char* home = getenv("HOME");
    string history(home);
    history.append("/.dtnet_history");

    /* Parse Command Line Arguments */
    int result = parseargs(argc, argv, verbose, script, procs);
    if (result != 0) return result;

    /* Display A Welcome Message */
    if (verbose) cout << endl << "Welcome to the Parallel Network Simulator 2.0" << endl;
    if (verbose) cout << "Written by Brandon Aubie <aubiebn@mcmaster.ca>" << endl;
    if (verbose) cout << "Compiled " << __DATE__ << " " << __TIME__ << endl;
	
    /* Setup Threads For Parallel Simulations */
    if (verbose) cout << "...Pooling " << procs << " threads for simulations";
    boost::threadpool::pool tp(procs);
    if (verbose) cout << "\t[OK]" << endl;
    


    /* Main Input Loop */
    if (verbose) cout << "Initialization Complete" << endl;
    if (verbose) cout << "Simulation is Empty" << endl << endl;

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
        dtlang::parse("external(\"" + script + "\")", tp, verbose, EndOfInput);
    }

    string input;
    while (!EndOfInput) {
        input = Reader.GetLine("dtnet> ", EndOfInput); 
        if (dtlang::parse(input, tp, verbose, EndOfInput)) {
            Reader.SaveHistory(history);
        }
    }
    tp.wait();
    return 0;
}





