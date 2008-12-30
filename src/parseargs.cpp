#include "parseargs.h"

using namespace std;

int parseargs(int argc, char* argv[], bool& verbose, string& script, int& procs, bool& gv) {

    for (int i=0; i < argc; i++) {

        if (strcmp(argv[i],"-h") == 0) {
            outputHelp();
            return 1;
        }
        else if (strcmp(argv[i],"-v") == 0) {
            verbose = true;
        }
        else if (strcmp(argv[i],"-gv") == 0) {
            gv = true;
        }
        else if (strcmp(argv[i],"-s") == 0) {
            i++;
            if (i >= argc) {
                cout << "ERROR: " << argv[i-1] << " takes an argument.\n";
                outputHelp();
                return(-1);
            }
            script = argv[i];
        }
        else if (strcmp(argv[i],"-p") == 0) {
            i++;
            if (i >= argc) {
                cout << "ERROR: " << argv[i-1] << " takes an argument.\n";
                outputHelp();
                return(-1);
            }
            procs = (int)atoi(argv[i]);
        }
    }

    // Sanity Checks
    if (procs <= 0) {
        cout << "ERROR: Maximum number of processes must be at least 1\n";
        outputHelp();
        return -1;
    }
    return 0;
}


void outputHelp() {
    cout << "Network Simulator\n";
    cout << "By Brandon Aubie <brandon@aubie.ca>\n";
    cout << "McMaster University, Ontario, Canada\n\n";
    cout << "Usage: dtnet [OPTIONS]\n";
    cout << "  -h\t\tDisplay this screen.\n";
    cout << "  -gv\t\tUse ghostview (gv) to preview graphs.\n";
    cout << "  -s filename\t Load a script to run upon startup.\n\n";
    cout << "  -p #procs\t Specify the maximum number of processes to spawn (Default 8).\n";
    cout << "  -v\t\tUse verbose output during the simulation.\n";
}


