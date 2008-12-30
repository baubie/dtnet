
#include "dtlang.h"

struct strCmp {
    bool operator()( const std::string &s1, const std::string &s2 ) const {
        return strcmp( s1.c_str(), s2.c_str() ) < 0;
    }
};

namespace dtlang {
    map<std::string, function_def> functions;
    map<std::string, variable_def> vars;
    map<int, std::string> type_names;
}


void dtlang::initialize() 
{
    dtlang::initialize_variables();
    dtlang::initialize_functions();
}

bool dtlang::params_to_variables(dtlang::parameters &params, vector<dtlang::variable_def> &var_params, boost::threadpool::pool &tp, bool &end_input)
{
    var_params.resize(params.size());
    dtlang::parameters::iterator param_iter;
    int count = 0;
    for (param_iter = params.begin(); param_iter != params.end(); ++param_iter) {
       if (!dtlang::parse_statement(*param_iter, var_params[count], true, true, tp, end_input))
       {
            // Something bad happened
            // Clean up the work we've already done.
            for (int i = 0; i < count -1; i++) {
                dtlang::delete_variable(var_params[i]);
            } 
            cout << "Unable to parse " << *param_iter << endl << endl;
            return false;
       }
       ++count; 
    }
    return true;
}

bool dtlang::parse(const string &str, boost::threadpool::pool &tp, bool &end_input)
{
	if (str == "") return true;

    bool r;
    string::const_iterator iter, end;

    // Check if it is a comment
    dtlang::comment_parser<string::const_iterator> pComment;
    iter = str.begin();
    end = str.end();
    r = phrase_parse(iter, end, pComment, boost::spirit::ascii::space); 
    if (r && iter == end) {
        return true;
    }

    // Check if it is an assignment
    dtlang::assignment_parser<string::const_iterator> pAssignment;
    dtlang::variable_assign var;
    iter = str.begin();
    end = str.end();
    r = phrase_parse(iter, end, pAssignment, var, boost::spirit::ascii::space);
    if (r && iter == end) {
        // It is an assignment!
        iter = var.value.begin();
        end = var.value.end();
        dtlang::variable_def new_var;
        if (dtlang::parse_statement(var.value, new_var, true, true, tp, end_input)) {
            if (dtlang::vars.find(var.name) != dtlang::vars.end()) dtlang::delete_variable(dtlang::vars[var.name]);
            dtlang::vars[var.name] = new_var;
            return true;
        }
        cout << "Syntax error: " << var.value << endl << endl;
        return false; 
    }

    // Perhaps it's just a function call?
    dtlang::variable_def dud_var; // A place holder
    if (!dtlang::parse_statement(str, dud_var, false, false, tp, end_input))
    {
        /* Failed To Parse */
        string::const_iterator some = iter+30;
        string context(iter, (some>end)?end:some);
        cout << "Syntax error: \"" << context << "...\"" << endl << "Type \"help()\" for assistance." << endl << endl;
        return false;
    }
    if (dud_var.type != dtlang::NO_RETURN && dud_var.type != dtlang::TYPE_VOID) {
        dtlang::delete_variable(dud_var);
    }
    return true;
}


bool dtlang::parse_statement(const string &str, variable_def &var, const bool assignment, const bool make_copy, boost::threadpool::pool &tp, bool &end_input)
{
    bool r;

    // Is it a function?
    dtlang::function_parser<string::const_iterator> pFunction;
    dtlang::function_call func;
    string::const_iterator iter = str.begin();
    string::const_iterator end = str.end();
    r = phrase_parse(iter, end, pFunction, func, boost::spirit::ascii::space);
    if (r && iter == end) {
        if (func.first_param != "") { func.params.insert(func.params.begin(), (string)func.first_param); }
        void *ret;
        int r_type = dtlang::NO_RETURN;
        if (assignment) r_type = NULL;
        vector<dtlang::variable_def> var_params; 
        if (!dtlang::params_to_variables(func.params, var_params, tp, end_input)) return false;
        bool run_result = dtlang::runFunction(func.name, var_params, tp, ret, r_type, end_input);
        var.type = r_type;
        var.obj = ret;
        // Clean up the parameters
        vector<dtlang::variable_def>::iterator iter;
        for (iter = var_params.begin(); iter != var_params.end(); ++iter) {
            dtlang::delete_variable(*iter);
        } 
		if (run_result == false) return false;
        if (func.name == "quit") { end_input = true; }
        return true;
    }

    // Is it a string?
    dtlang::string_parser<string::const_iterator> pString;
    iter = str.begin();
    end = str.end();
    string *str_val = new string();
    r = phrase_parse(iter, end, pString, *str_val, boost::spirit::ascii::space);
    if (r && iter == end) { var.type = dtlang::TYPE_STRING; var.obj = str_val; return true; }
    delete str_val;

    // Is it an integer?
    dtlang::integer_parser<string::const_iterator> pInteger;
    iter = str.begin();
    end = str.end();
    int *int_val = new int();
    r = phrase_parse(iter, end, pInteger, *int_val, boost::spirit::ascii::space);
    if (r && iter == end) { var.type = dtlang::TYPE_INT; var.obj = int_val; return true; }
    delete int_val;

    // Is it a double?
    dtlang::double_parser<string::const_iterator> pDouble;
    iter = str.begin();
    end = str.end();
    double *double_val = new double();
    r = phrase_parse(iter, end, pDouble, *double_val, boost::spirit::ascii::space);
    if (r && iter == end) { var.type = dtlang::TYPE_DOUBLE; var.obj = double_val; return true; }
    delete double_val;

    // We'll assume it's a variable
    if (dtlang::vars.find(str) != dtlang::vars.end()) {
        if (make_copy) {
            variable_def oldvar = dtlang::vars[str];
            var.type = oldvar.type;
            switch(oldvar.type) {
                case dtlang::TYPE_STRING:
                    var.obj = new string(*(static_cast<string*>(oldvar.obj)));
                    break;

                case dtlang::TYPE_INT:
                    var.obj = new int(*(static_cast<int*>(oldvar.obj)));
                    break;

                case dtlang::TYPE_DOUBLE:
                    var.obj = new double(*(static_cast<double*>(oldvar.obj)));
                    break;

                case dtlang::TYPE_TRIAL:
                    var.obj = new Trial(*(static_cast<Trial*>(oldvar.obj)));
                    break;
                
                case dtlang::TYPE_NET:
                    var.obj = new Net(*(static_cast<Net*>(oldvar.obj)));
                    break;

                default:
                    cout << "Unable to determine variable type for copying." << endl;
                    return false;
                    break;
            } 

        } else {
            // We create a new variable
            // But the *obj pointer is the same
            // So any changes to this variable change the first
            var = dtlang::vars[str];
        }
        return true;
    }

    return false;
}

void dtlang::initialize_variables()
{
    dtlang::variable_def v;

    dtlang::type_names[dtlang::TYPE_VOID] = "Void";
    dtlang::type_names[dtlang::TYPE_ANY] = "*ANY*";
    dtlang::type_names[dtlang::TYPE_DOUBLE] = "Double";
    dtlang::type_names[dtlang::TYPE_INT] = "Integer";
    dtlang::type_names[dtlang::TYPE_POPULATION] = "Population";
    dtlang::type_names[dtlang::TYPE_STRING] = "String";
    dtlang::type_names[dtlang::TYPE_TRIAL] = "Trial";
    dtlang::type_names[dtlang::TYPE_NET] = "Net";

    v.type = dtlang::TYPE_DOUBLE;
    v.obj = new double(0.05);
    dtlang::vars["dt"] = v;

    v.type = dtlang::TYPE_DOUBLE;
    v.obj = new double(50);
    dtlang::vars["T"] = v;

    v.type = dtlang::TYPE_DOUBLE;
    v.obj = new double(10);
    dtlang::vars["delay"] = v;
}

void dtlang::initialize_functions()
{
    dtlang::function_def f;
    dtlang::function_param p;

    // quit()
    f.help = "Quit to the command prompt.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    dtlang::functions["quit"] = f;
    
    // help()
    f.help = "Display general usage information or help for a particular function.";
    f.return_type = dtlang::TYPE_VOID; 
    f.params.clear();
    p.type = dtlang::TYPE_STRING;
    p.help = "Name of the function to get help for.";
    p.optional = true;
    f.params["name"] = p;
    dtlang::functions["help"] = f;

    // vars()
    f.help = "List the variables currently stored in the register.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    dtlang::functions["vars"] = f;

    // funcs()
    f.help = "List the functions available for use.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    dtlang::functions["funcs"] = f;

    // benchmark()
    f.help = "Run a simple, multi-threaded benchmarking function.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    p.type = dtlang::TYPE_DOUBLE;
    p.help = "Number of times to run the benchmark.  Higher numbers result in longer benchmark times.";
    p.optional = true;
    p.def = "1";
    f.params["multiplier"] = p;
    dtlang::functions["benchmark"] = f;

    // loadtrial()
    f.help = "Load and return a trial from a trial XML file. This function must be assigned to a variable.";
    f.return_type = dtlang::TYPE_TRIAL;
    f.params.clear();
    p.type = dtlang::TYPE_STRING;
    p.help = "Filename of the trial XML file.";
    p.optional = false;
    f.params["filename"] = p;
    dtlang::functions["loadtrial"] = f;

    // loadnetwork()
    f.help = "Load and return a network from a network XML file. This function must be assigned to a variable.";
    f.return_type = dtlang::TYPE_NET;
    f.params.clear();
    p.type = dtlang::TYPE_STRING;
    p.help = "Filename of the network XML file.";
    p.optional = false;
    f.params["filename"] = p;
    dtlang::functions["loadnetwork"] = f;

    // graphinputs()
    f.help = "Display a graph showing some or all of the inputs from a trial.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    p.type = dtlang::TYPE_TRIAL;
    p.help = "A trial variable from an already loaded trial.";
    p.optional = false;
    f.params["trial"] = p;
    p.type = dtlang::TYPE_STRING;
    p.help = "Filename to save the generated postscript file to.";
    p.optional = true;
    p.def = "inputs.ps";
    f.params["filename"] = p;
    dtlang::functions["graphinputs"] = f;

    // external()
    f.help = "Execute a series of commands as stored in an external file.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    p.type = dtlang::TYPE_STRING;
    p.help = "Filename of the external file.";
    p.optional = false;
    f.params["filename"] = p;
    dtlang::functions["external"] = f;

    // print()
    f.help = "Print the contents of the variable by either simply printing it or calling the corresponding C++ function \"toString()\", depending on the type.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    p.type = dtlang::TYPE_ANY;
    p.help = "Variable to print.";
    p.optional = false;
    f.params["variable"] = p;
    dtlang::functions["print"] = f;
}

bool dtlang::runFunction(const string &name, const vector<variable_def> &params, boost::threadpool::pool &tp, void *&r, int &r_type, bool &end_input)
{
    dtlang::function_def f;
    map<string, function_def>::iterator iter = dtlang::functions.find(name);

    if (iter == dtlang::functions.end()) {
        cout << "Error: Unknown function" << endl;
        return false;
    } 
    f = iter->second;

    unsigned int req = 0;
    unsigned int opt = 0;
    map<string, function_param>::iterator iter_param;
    for (iter_param = f.params.begin(); iter_param != f.params.end(); ++iter_param) {
        if (iter_param->second.optional == false) ++req;
        if (iter_param->second.optional == true)  ++opt;
    }
    if (params.size() < req || params.size() > req+opt) {
        cout << "Error: Incorrect number of function parameters.\nType \"help(\"" << name << "\")\" for more information." << endl;
        return false;
    }
    //TODO: Check if paramter type is correct

    if (r_type != dtlang::NO_RETURN) {
        r_type = dtlang::functions[name].return_type; 
    }

    // Execute Functions
	if (name == "quit") {
        return dtlang::f_quit(tp);
	} 

	if (name == "help") {
        if (params.size() == 0) {
            return dtlang::f_help();
        } else if (params.size() == 1) {
            return dtlang::f_help(*(static_cast<string*>(params[0].obj)));
        }
	} 

	if (name == "graphinputs") {
        if (params.size() == 1) {
            return dtlang::f_graphinputs(*(static_cast<Trial*>(params[0].obj)), "inputs.ps");
        } else if (params.size() == 2) {
            return dtlang::f_graphinputs(*(static_cast<Trial*>(params[0].obj)), *(static_cast<string*>(params[1].obj)));
        }
	} 

	if (name == "vars") {
        return dtlang::f_vars();
	} 
	
	if (name == "funcs") {
        return dtlang::f_funcs();
	} 

	if (name == "print") {
        return dtlang::f_print(params[0].obj, params[0].type);
	} 
	
	if (name == "benchmark") {
        if (params.size() == 0) {
            return dtlang::f_benchmark(tp, 1.0);
        } else if (params.size() == 1) {
            return dtlang::f_benchmark(tp, *(static_cast<double*>(params[0].obj)));
        }
	}

	if (name == "external") {
        return dtlang::f_external(*(static_cast<string*>(params[0].obj)), tp, end_input);
	}

    if (name == "loadnetwork") {
        if (r_type == dtlang::NO_RETURN) {
            cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
            return false;
        }
        r = new Net();
        return dtlang::f_loadnetwork(*(static_cast<string*>(params[0].obj)), static_cast<Net*>(r));
    }

	if (name == "loadtrial") {
        if (r_type == dtlang::NO_RETURN) {
            cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
            return false;
        }
        if (dtlang::vars.find("T") == dtlang::vars.end() || dtlang::vars["T"].type != dtlang::TYPE_DOUBLE) { cout << "Error: T must be a double!" << endl; return false;}
        if (dtlang::vars.find("dt") == dtlang::vars.end() || dtlang::vars["dt"].type != dtlang::TYPE_DOUBLE) { cout << "Error: dt must be a double!" << endl; return false;}
        if (dtlang::vars.find("delay") == dtlang::vars.end() || dtlang::vars["delay"].type != dtlang::TYPE_DOUBLE) { cout << "Error: delay must be a double!" << endl; return false;}
        r = new Trial(*(static_cast<double*>(dtlang::vars["T"].obj)),
                       *(static_cast<double*>(dtlang::vars["dt"].obj)),
                       *(static_cast<double*>(dtlang::vars["delay"].obj))
                      );
        return dtlang::f_loadtrial(*(static_cast<string*>(params[0].obj)), static_cast<Trial*>(r));
	} 

    return false;
}





bool dtlang::f_vars() {

    cout << endl << "Variable" << "\t" << "Type" << "\t" << "Value" << endl;
    cout << "------------------------------------" << endl;
    map<string, variable_def, strCmp>::iterator iter;
    for (iter = dtlang::vars.begin(); iter != dtlang::vars.end(); ++iter) {
        cout << iter->first << "\t\t";
        cout << dtlang::type_names[iter->second.type] << "\t";

        switch(iter->second.type) {
            case dtlang::TYPE_STRING:
                cout << *(static_cast<string*>(iter->second.obj));
                break;

            case dtlang::TYPE_INT:
                cout << *(static_cast<int*>(iter->second.obj));
                break;

            case dtlang::TYPE_DOUBLE:
                cout << *(static_cast<double*>(iter->second.obj));
                break;

            case dtlang::TYPE_TRIAL:
                cout << "[Trial Object]";
                break;
            
            case dtlang::TYPE_NET:
                cout << "[Net Object]";
                break;

            default:
                cout << "Unknown";
                break;
        } 
        cout << endl;
    }
    cout << endl;
    return true;
}

bool dtlang::f_funcs() {

    cout << endl << "Function" << "\t" << "Type" << "\t" << "Description" << endl;
    cout << "------------------------------------" << endl;
    map<string, function_def, strCmp>::iterator iter;
    for (iter = dtlang::functions.begin(); iter != dtlang::functions.end(); ++iter) {
        cout << iter->first << "\t";
        if (iter->first.length() < 8) cout << "\t";
        cout << dtlang::type_names[iter->second.return_type] << "\t";
        cout << iter->second.help << endl; 
    }
    cout << endl;
    return true;
}

bool dtlang::f_help() {
    cout << endl << "This is the command line where you can run functions or assign values to variables." << endl;
    cout << "To call a function, type the function name, followed by a (, a list of parameters, and then a )." << endl;
    cout << "For example, to quit back to the command prompt, type \"quit()\" and press ENTER." << endl;
    cout << "To get help on a function, type \"help(functionname)\", such as \"help(loadinput)\"." << endl;
    cout << "To change the timestep to 0.01 ms, type \"dt = 0.01\" and press ENTER." << endl;
    cout << "A series of commands can also be stored in a file and run in sequence by typing \"external(file)\"." << endl;
    cout << "Type \"funcs()\" to see a list of available functions." << endl;
    cout << endl;
    return true;
}

bool dtlang::f_help(string name) {

    dtlang::function_def f;
    map<string, function_def>::iterator iter = dtlang::functions.find(name);

    if (iter == dtlang::functions.end()) {
        cout << "Cannot find a function named \"" << name << "\"" << endl << endl;
        return false;
    } 
    f = iter->second;
    map<string, function_param>::iterator iter_param;

    cout << endl << name << "(";
    int opt = 0;
    for (iter_param = f.params.begin(); iter_param != f.params.end(); ++iter_param) {
        if (iter_param != f.params.begin()) cout << ", ";
        if (iter_param->second.optional) { ++opt; cout << '['; }
        cout << "<" << iter_param->first << ":" << dtlang::type_names[iter_param->second.type] << ">";
    }
    for (int i = 0; i < opt; ++i) { cout << "]"; };
    cout << ")" << endl;
    cout << f.help << endl;

    for (iter_param = f.params.begin(); iter_param != f.params.end(); ++iter_param) {
        cout << "<" << iter_param->first << ":" << iter_param->second.type << ">";
        if (iter_param->second.optional) {
            cout << "\t*Optional* (Default: " << iter_param->second.def << ")" << endl;
        } else {
            cout << "\t[Required]" << endl;
        }
        cout << "\t" << iter_param->second.help << endl; 
    } 
    cout << endl;
    return true;
}


bool dtlang::f_quit(boost::threadpool::pool &tp) {
    tp.wait();
    map<string, variable_def>::iterator iter;
    for (iter = vars.begin(); iter != vars.end(); ++iter) {
        dtlang::delete_variable(vars[iter->first]);
    }
    cout << endl;
    return true;
}

bool dtlang::delete_variable(variable_def var) {
    /**
     * Delete the object a variable points to.
     */
    switch (var.type) {
        case dtlang::TYPE_STRING:
            delete static_cast<string*>(var.obj);
            break;

        case dtlang::TYPE_INT:
            delete static_cast<int*>(var.obj);
            break;

        case dtlang::TYPE_DOUBLE:
            delete static_cast<double*>(var.obj);
            break;

        case dtlang::TYPE_TRIAL:
            delete static_cast<Trial*>(var.obj);
            break;
        
        case dtlang::TYPE_NET:
            delete static_cast<Net*>(var.obj);
            break;

        default:
            cout << "Attempted to free an unknown variable type.  MEMORY LEAK!" << endl;
            return false;
            break;
    }
    return true;
}


bool dtlang::f_runsimulation(Input input, Net net) {

    return true;
}


void __benchmark(double h) {
    double i = 2524224.942842;
    double j = 2431.204820482;
    double k = 1141.2394293333231113934893;
    double r;
    for (double x = 0; x <= 100000; ++x) { r = ((i+j/h)*k + (x*1942.4194814)) / j; }
}
bool dtlang::f_benchmark(boost::threadpool::pool &tp, double mult) {

    boost::posix_time::ptime start(boost::posix_time::microsec_clock::local_time());
    for (int i = 0; i <= (int)(100000*mult); ++i) {
        tp.schedule(boost::bind(&__benchmark, (double)i));
    }
    tp.wait();
    boost::posix_time::ptime end(boost::posix_time::microsec_clock::local_time());
    boost::posix_time::time_duration dur = end - start;
    cout << dur << endl << endl;
    return true;
}

bool dtlang::f_external(const string filename, boost::threadpool::pool &tp, bool &end_input) {
    string line;
    ifstream script(filename.c_str());
    if (script.is_open()) 
    {
        while (!script.eof() && !end_input) 
        {
            getline(script, line);    
			if (line != "") // Skip blank lines
			{
				cout << VT_set_colors(VT_RED, VT_DEFAULT) << "extern" << VT_default_attributes << "> " << line << endl;
				if (!dtlang::parse(line, tp, end_input))
				{
					cout << "Error encountered in script.  Halting execution." << endl << endl;
					break;
				}
			} 
        }

        script.close();
        return false;
    } else {
        cout << "Unable to open " << filename << "." << endl << endl;
        return false;
    }
}

bool dtlang::f_loadnetwork(const string filename, Net *net) {

    string error;
    /* Load Network */
    if (dtlang::verbose) cout << "...Loading network definition from " << filename;
    if ( net->load(filename, error) == false ) {
        if (dtlang::verbose) cout << "\t[FAILED]" << endl;
		cout << "[X] " << error << endl;
        return false;
    }
    if (dtlang::verbose) cout << "\t[OK]" << endl;
    if (dtlang::verbose) cout << "...Loaded " << net->count_populations() << " populations" << endl;
    return true;
}


bool dtlang::f_loadtrial(const string filename, Trial *trial) {

    string error;
    /* Load Inputs */
    if (dtlang::verbose) cout << "...Loading input vectors from " << filename;
    if ( trial->load(filename, error) == false ) {
        if (dtlang::verbose) cout << "\t[FAILED]" << endl;
        cout << "[X] " << error << endl;
        return false;
    }
    if (dtlang::verbose) cout << "\t[OK]" << endl;
    if (dtlang::verbose) cout << "...Generated " << trial->count() << " input signals" << endl;
    return true;
}

bool dtlang::f_graphinputs(Trial &trial, string const &filename) {
    vector<vector<double> >* signals = trial.signals();
    vector<double>* timesteps = trial.timeSteps();

    GLE gle;
    GLE::PlotProperties plotProperties;
    GLE::Color start;

    start.r = 0.5;
    start.g = 0.5;
    start.b = 0.5;
    plotProperties.first = start;
    gle.plot(*timesteps, *signals, plotProperties);

    gle.canvasProperties.width = 8.5;
    gle.canvasProperties.height = 7;
    gle.draw(filename);
    return true;
}

bool dtlang::f_print(void* ptr, int const type)
{
    switch(type)
    {
        case dtlang::TYPE_STRING:
            cout << *(static_cast<string*>(ptr)) << endl;
            break;

        case dtlang::TYPE_TRIAL:
            cout << static_cast<Trial*>(ptr)->toString();
            break;

        case dtlang::TYPE_NET:
            cout << static_cast<Net*>(ptr)->toString();
            break;    

        case dtlang::TYPE_INT:
            cout << *(static_cast<int*>(ptr)) << endl;
            break;    

        case dtlang::TYPE_DOUBLE:
            cout << *(static_cast<double*>(ptr)) << endl;
            break;        
            
        default:
            cout << "Cannot print this data type." << endl;
            return false;
            break;
    }
    return true;

}

