/*	Parallel Network Simulator
 *  Written by Brandon Aubie
 *  aubiebn@mcmaster.ca
 */

#include "dtnet.h"

using namespace std;
using namespace swift;
namespace po = boost::program_options;


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
    po::options_description generic("Generic options");
    generic.add_options()
        ("help,h", "produce help message")
        ("verbose,v", "show more messages while the program is running")
        ("script,s", po::value<string>(), "set a script to run right away")
    ;

    po::options_description config("Configuration");
    config.add_options()
        ("eps", po::value<string>(&GLE::viewer), "specify a program to view eps files with")
        ("procs,p", po::value<int>(&procs)->default_value(1), "set the number of threads available for simulations")
        ;

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(visible).run(), vm);
    po::notify(vm);

    string config_file(home);
    config_file.append("/.dtnetrc");
    ifstream ifs(config_file.c_str());
    po::store(po::parse_config_file(ifs, config), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << visible << "\n";
        return 1;
    }
    if (vm.count("verbose")) dtlang::verbose = true; 

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





