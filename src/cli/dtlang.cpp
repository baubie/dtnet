
#include "dtlang.h"

using namespace std;

struct strCmp {
    bool operator()( const std::string &s1, const std::string &s2 ) const {
        return strcmp( s1.c_str(), s2.c_str() ) < 0;
    }
};

namespace dtlang {
    map<std::string, function_def> functions;
    map<std::string, variable_def> vars;
    map<std::string, variable_def> statements;
    map<int, std::string> type_names;
}

void dtlang::initialize()
{
    dtlang::initialize_variables();
    dtlang::initialize_functions();
}

bool dtlang::params_to_variables(dtlang::parameters &params, vector<dtlang::variable_def> &var_params, bool &end_input)
{
    var_params.resize(params.size());
    dtlang::parameters::iterator param_iter;
    int count = 0;
    for (param_iter = params.begin(); param_iter != params.end(); ++param_iter) {
       if (!dtlang::parse_statement(*param_iter, var_params[count], true, true, end_input))
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

bool dtlang::parse(const std::string &str, bool &end_input) {

    if (str == "") return true;

    bool r;
	std::string::const_iterator iter, end;

    // Check if it is a comment
    dtlang::comment_parser<std::string::const_iterator> pComment;
    iter = str.begin();
    end = str.end();
    r = phrase_parse(iter, end, pComment, boost::spirit::ascii::space);
    if (r && iter == end) {
        return true;
    }

    // Check if it is an assignment
    dtlang::assignment_parser<std::string::const_iterator> pAssignment;
    dtlang::variable_assign var;
    iter = str.begin();
    end = str.end();
    r = phrase_parse(iter, end, pAssignment, boost::spirit::ascii::space, var);
    if (r && iter == end) {
        // It is an assignment!
        iter = var.value.begin();
        end = var.value.end();
        dtlang::variable_def new_var;
        if (dtlang::parse_statement(var.value, new_var, true, true, end_input)) {
            if (dtlang::vars.find(var.name) != dtlang::vars.end()) dtlang::delete_variable(dtlang::vars[var.name]);
            dtlang::vars[var.name] = new_var;
            return true;
        }
        cout << "Syntax error: " << var.value << endl << endl;
        return false;
    }

    // Perhaps it's just a function call?
    dtlang::variable_def dud_var; // A place holder
    if (!dtlang::parse_statement(str, dud_var, false, false, end_input))
    {
        /* Failed To Parse */
		std::string::const_iterator some = iter+30;
		std::string context(iter, (some>end)?end:some);
        cout << "Syntax error: \"" << context << "...\"" << endl << "Type \"help()\" for assistance." << endl << endl;
        return false;
    }
    if (dud_var.type != dtlang::NO_RETURN && dud_var.type != dtlang::TYPE_VOID) {
        dtlang::delete_variable(dud_var);
    }

    return true;
}


bool dtlang::parse_statement(const std::string &str, variable_def &var, const bool assignment, const bool make_copy, bool &end_input) {

    bool r;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();

    // Is it a function?
    dtlang::function_parser<std::string::const_iterator> pFunction;
    dtlang::function_call func;
    r = phrase_parse(iter, end, pFunction, boost::spirit::ascii::space, func);
    if (r && iter == end) {
        if (func.first_param != "") { func.params.insert(func.params.begin(), (std::string)func.first_param); }
        void *ret;
        int r_type = dtlang::NO_RETURN;
        if (assignment) r_type = dtlang::TYPE_VOID;
        vector<dtlang::variable_def> var_params;
        if (!dtlang::params_to_variables(func.params, var_params, end_input)) return false;
        bool run_result = dtlang::runFunction(func.name, var_params, ret, r_type, end_input);
        var.type = r_type;
        var.obj = ret;
        // Clean up the parameters
        vector<dtlang::variable_def>::iterator iter;
        for (iter = var_params.begin(); iter != var_params.end(); ++iter) {
            dtlang::delete_variable(*iter);
        }
        if (func.name == "quit") { end_input = true; }
        return true;
    }

    // Is it a string?
    dtlang::string_parser<std::string::const_iterator> pString;
    iter = str.begin();
    end = str.end();
	std::string *str_val = new std::string();
    r = phrase_parse(iter, end, pString, boost::spirit::ascii::space, *str_val);
    if (r && iter == end) { var.type = dtlang::TYPE_STRING; var.obj = str_val; return true; }
    delete str_val;

    // Is it a double?
    dtlang::double_parser<std::string::const_iterator> pDouble;
    iter = str.begin();
    end = str.end();
    double *double_val = new double();
    r = phrase_parse(iter, end, pDouble, boost::spirit::ascii::space, *double_val);
    if (r && iter == end) { var.type = dtlang::TYPE_DOUBLE; var.obj = double_val; return true; }
    delete double_val;

    // Is it an integer?
    dtlang::integer_parser<std::string::const_iterator> pInteger;
    iter = str.begin();
    end = str.end();
    int *int_val = new int();
    r = phrase_parse(iter, end, pInteger, boost::spirit::ascii::space, *int_val);
    if (r && iter == end) { var.type = dtlang::TYPE_INT; var.obj = int_val; return true; }
    delete int_val;

    // Is it a statement?
    if (dtlang::statements.find(str) != dtlang::statements.end()) {
        if (make_copy) {
            variable_def oldvar = dtlang::statements[str];
            var.type = oldvar.type;
            switch(oldvar.type) {
                case dtlang::TYPE_STRING:
                    var.obj = new std::string(*(static_cast<std::string*>(oldvar.obj)));
                    break;

                case dtlang::TYPE_INT:
                    var.obj = new int(*(static_cast<int*>(oldvar.obj)));
                    break;

                case dtlang::TYPE_DOUBLE:
                    var.obj = new double(*(static_cast<double*>(oldvar.obj)));
                    break;

				default:
                    cout << "Unable to determine statement type for copying." << endl;
                    return false;
                    break;
            }

        } else {
            var = dtlang::statements[str];
            return true;
        }
        return true;
    }

    // We'll assume it's a variable
    if (dtlang::vars.find(str) != dtlang::vars.end()) {
        if (make_copy) {
            variable_def oldvar = dtlang::vars[str];
            var.type = oldvar.type;
            switch(oldvar.type) {
                case dtlang::TYPE_STRING:
                    var.obj = new std::string(*(static_cast<std::string*>(oldvar.obj)));
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

                case dtlang::TYPE_RESULTS:
                    var.obj = new Results(*(static_cast<Results*>(oldvar.obj)));
                    break;

				case dtlang::TYPE_SIMULATION:
					var.obj = new Simulation(*(static_cast<Simulation*>(oldvar.obj)));
					break;

				case dtlang::TYPE_POPULATION:
					var.obj = new Population(*(static_cast<Population*>(oldvar.obj)));
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
	dtlang::type_names[dtlang::TYPE_SIMULATION] = "Simulation";
	dtlang::type_names[dtlang::TYPE_RESULTS] = "Results";

    v.type = dtlang::TYPE_DOUBLE;
    v.obj = new double(0);
    dtlang::statements["false"] = v;

    v.type = dtlang::TYPE_DOUBLE;
    v.obj = new double(1);
    dtlang::statements["true"] = v;
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
    p.name = "name";
    f.params.push_back(p);

    dtlang::functions["help"] = f;


    // set()
    f.help = "Set a system configuration variable.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();

    p.type = dtlang::TYPE_STRING;
    p.help = "Variable name";
    p.optional = false;
    p.name = "variable";
    f.params.push_back(p);

    p.type = dtlang::TYPE_DOUBLE;
    p.help = "Variable value";
    p.optional = false;
    p.name = "value";
    f.params.push_back(p);

    dtlang::functions["set"] = f;


    // modsim()
    f.help = "Modify a simulation parameter with a single value after it has been loaded.";
    f.return_type = dtlang::TYPE_SIMULATION;
    f.params.clear();

    p.type = dtlang::TYPE_SIMULATION;
    p.help = "Simulation to modify.";
    p.optional = false;
    p.name = "simulation";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Parameter ID";
    p.optional = false;
    p.name = "ID";
    f.params.push_back(p);

    p.type = dtlang::TYPE_DOUBLE;
    p.help = "Value of the parameter";
    p.optional = false;
    p.name = "value";
    f.params.push_back(p);

    dtlang::functions["modsim"] = f;

    //modsimr()
    f.help = "Modify a simulation parameter with a range of values after it has been loaded.";
    f.return_type = dtlang::TYPE_SIMULATION;
    f.params.clear();

    p.type = dtlang::TYPE_SIMULATION;
    p.help = "Simulation to modify.";
    p.optional = false;
    p.name = "simulation";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Parameter ID";
    p.optional = false;
    p.name = "ID";
    f.params.push_back(p);

    p.type = dtlang::TYPE_DOUBLE;
    p.help = "Value of the range start.";
    p.optional = false;
    p.name = "start";
    f.params.push_back(p);

    p.type = dtlang::TYPE_DOUBLE;
    p.help = "Value of the range end.";
    p.optional = false;
    p.name = "end";
    f.params.push_back(p);

    p.type = dtlang::TYPE_DOUBLE;
    p.help = "Value of the range step value.";
    p.optional = false;
    p.name = "step";
    f.params.push_back(p);

    dtlang::functions["modsimr"] = f;

    // vars()
    f.help = "List the variables currently stored in the register.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    dtlang::functions["vars"] = f;

    // version()
    f.help = "Display the current version.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    dtlang::functions["version"] = f;

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
    p.name="multiplier";
    f.params.push_back(p);

    dtlang::functions["benchmark"] = f;


    // simulation()
	f.help = "Initalize a simulation with a particular network.";
	f.return_type = dtlang::TYPE_SIMULATION;
	f.params.clear();

	p.type = dtlang::TYPE_STRING;
	p.help = "Filename of a network XML file.";
	p.optional = false;
    p.name = "network";
	f.params.push_back(p);

	p.type = dtlang::TYPE_STRING;
	p.help = "Filename of a trial XML file.";
	p.optional = false;
    p.name = "trial";
	f.params.push_back(p);

	dtlang::functions["simulation"] = f;


    // run()
    f.help = "Run the current simulation. Will only work if at least one population has an input connected.";
    f.return_type = dtlang::TYPE_RESULTS;
    f.params.clear();

	p.type = dtlang::TYPE_SIMULATION;
	p.help = "The simulation you wish to run.";
	p.optional = false;
    p.name="simulation";
    f.params.push_back(p);

	p.type = dtlang::TYPE_STRING;
	p.help = "Filename to save results to.";
	p.optional = false;
    p.name="filename";
    f.params.push_back(p);

	p.type = dtlang::TYPE_INT;
	p.help = "Number of simulations to run for each input vector.";
	p.optional = false;
    p.name = "number_of_trials";
    f.params.push_back(p);

	p.type = dtlang::TYPE_DOUBLE;
	p.help = "Number of milliseconds to delay before time zero and the start of a trial.";
	p.optional = true;
    p.def = "10";
    p.name = "delay";
    f.params.push_back(p);

	p.type = dtlang::TYPE_INT;
	p.help = "Set to false if you do not wish to store the voltage parameters.";
	p.optional = true;
    p.def = "1";
    p.name = "store_voltage";
    f.params.push_back(p);

    dtlang::functions["run"] = f;

    // load()
    f.help = "Load a simulation that has already been run.";
    f.return_type = dtlang::TYPE_RESULTS;
    f.params.clear();

    p.type = dtlang::TYPE_STRING;
    p.help = "The filename where the simulation was saved.";
    p.optional = false;
    p.name = "filename";
    f.params.push_back(p);

    dtlang::functions["load"] = f;

    // constrain()
    f.help = "Constrain a results object along one of the unconstrained dimensions.";
    f.return_type = dtlang::TYPE_RESULTS;
    f.params.clear();

    p.type = dtlang::TYPE_RESULTS;
    p.help = "The results object that you wish to constrain.";
    p.optional = false;
    p.name = "results";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "The ID of the unconstrained dimension. Type print(your_results) to list these.";
    p.optional = false;
    p.name = "ID";
    f.params.push_back(p);

    p.type = dtlang::TYPE_DOUBLE;
    p.help = "The value you wish to constrain this dimension to.";
    p.optional = false;
    p.name = "value";
    f.params.push_back(p);

    dtlang::functions["constrain"] = f;

    // merge()
    f.help = "Merge two results objects.";
    f.return_type = dtlang::TYPE_RESULTS;
    f.params.clear();

    p.type = dtlang::TYPE_RESULTS;
    p.help = "First result to merge.  Can already be a merged result.";
    p.optional = false;
    p.name = "results1";
    f.params.push_back(p);

    p.type = dtlang::TYPE_RESULTS;
    p.help = "Second result to merge. Must NOT be a merged result yet.";
    p.optional = false;
    p.name = "results2";
    f.params.push_back(p);

    dtlang::functions["merge"] = f;

    // graphinputs()
    f.help = "Display a graph showing some or all of the inputs from a trial. Uses the graph_width and graph_height variables.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();

    p.type = dtlang::TYPE_TRIAL;
    p.help = "A trial variable from an already loaded trial.";
    p.optional = false;
    p.name = "trial";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Filename to save the generated postscript file to.";
    p.optional = true;
    p.def = "inputs.eps";
    p.name = "filename";
    f.params.push_back( p );

    //dtlang::functions["graphinputs"] = f;

    // graphspiketrains()
    f.help = "Display spike trains for every trial of the given population.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();

    p.type = dtlang::TYPE_RESULTS;
    p.help = "A results variable constrained to a single dimension.";
    p.optional = false;
    p.name = "results";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Population ID to plot.";
    p.optional = false;
    p.name = "populationID";
    f.params.push_back( p );

    p.type = dtlang::TYPE_STRING;
    p.help = "Filename to save the generated postscript file to.";
    p.optional = true;
    p.def = "spikes.eps";
    p.name = "filename";
    f.params.push_back( p );

    p.type = dtlang::TYPE_INT;
    p.help = "Number of trials to print.";
    p.optional = true;
    p.def = "Every Trial";
    p.name = "trials";
    f.params.push_back( p );

    p.type = dtlang::TYPE_INT;
    p.help = "Time value to start at.";
    p.optional = true;
    p.def = "Results timeseries start.";
    p.name = "start";
    f.params.push_back( p );

    p.type = dtlang::TYPE_INT;
    p.help = "Time value to end at.";
    p.optional = true;
    p.def = "Results timeseries end.";
    p.name = "end";
    f.params.push_back( p );

    dtlang::functions["graphspiketrains"] = f;

	// graphnetwork()
	f.help = "Produce a network flow diagram of the network.";
	f.return_type = dtlang::TYPE_VOID;
	f.params.clear();

	p.type = dtlang::TYPE_RESULTS;
	p.help = "Results from a block of simulations already run.";
	p.optional = false;
    p.name = "network";
	f.params.push_back(p);

	p.type = dtlang::TYPE_STRING;
	p.help = "Filename to save the generated postscript file to.";
	p.optional = true;
	p.def = "network.eps";
    p.name = "filename";
	f.params.push_back(p);
	dtlang::functions["graphnetwork"] = f;

    // graphspikecounts()
    f.help = "Plot mean spike counts over all trials in two dimensions (one on x-axis, on as separate series).";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();

    p.type = dtlang::TYPE_RESULTS;
    p.help = "A results object constrained to have one or two free parameters.";
    p.optional = false;
    p.name = "results";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Population ID to graph.";
    p.optional = false;
    p.name = "popID";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "ID of the dimension to graph on the x-axis.";
    p.optional = false;
    p.name = "x_axis";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Filename to save the graph to.";
    p.optional = false;
    p.name = "filename";
    f.params.push_back(p);
    dtlang::functions["graphspikecounts"] = f;


    // graphspikecounts3d()
    dtlang::functions["graphspikecounts3d"] = f;

    // graphspikecountsmap()
    dtlang::functions["graphspikecountsmap"] = f;

    // graphfirstspikelatency()
    f.help = "Plot mean first spike latency over all trials in two dimensions (one on x-axis, on as separate series).";
    dtlang::functions["graphfirstspikelatency"] = f;

    // graphpsth()
    f.help = "Produce a plot of the peri-stimulus time histogram";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();

    p.type = dtlang::TYPE_RESULTS;
    p.help = "A results object constrained to have no free parameters.";
    p.optional = false;
    p.name = "results";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Population ID to graph.";
    p.optional = false;
    p.name = "popID";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Filename to save the graph to.";
    p.optional = false;
    p.name = "filename";
    f.params.push_back(p);
    dtlang::functions["graphpsth"] = f;

    // graphtrial_voltage()
    f.help = "Produce a plot of the voltage traces for each neuron in each population.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();

    p.type = dtlang::TYPE_RESULTS;
    p.help = "A results object constrained to have no free parameters.";
    p.optional = false;
    p.name = "results";
    f.params.push_back(p);

    p.type = dtlang::TYPE_INT;
    p.help = "The trial number (zero based).";
    p.optional = false;
    p.name = "trial_number";
    f.params.push_back(p);

    p.type = dtlang::TYPE_STRING;
    p.help = "Filename to save the graph to.";
    p.optional = false;
    p.name = "filename";
    f.params.push_back(p);

    dtlang::functions["graphtrial_voltage"] = f;

    // graphtrial_spikes()
    dtlang::functions["graphtrial_spikes"] = dtlang::functions["graphtrial_voltage"];

    // external()
    f.help = "Execute a series of commands as stored in an external file.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    p.type = dtlang::TYPE_STRING;
    p.help = "Filename of the external file.";
    p.optional = false;
    p.name = "filename";
    f.params.push_back(p);
    dtlang::functions["external"] = f;

    // print()
    f.help = "Print the contents of the variable.";
    f.return_type = dtlang::TYPE_VOID;
    f.params.clear();
    p.type = dtlang::TYPE_ANY;
    p.help = "Variable to print.";
    p.optional = false;
    p.name = "variable";
    f.params.push_back(p);
    dtlang::functions["print"] = f;
}

bool dtlang::runFunction(const std::string &name, const vector<variable_def> &params, void *&r, int &r_type, bool &end_input)
{
    dtlang::function_def f;
    map<std::string, function_def>::iterator iter = dtlang::functions.find(name);

    if (iter == dtlang::functions.end()) {
        cout << "Error: Unknown function" << endl;
        return false;
    }
    f = iter->second;

    unsigned int req = 0;
    unsigned int opt = 0;
    vector<function_param>::iterator iter_param;
    for (iter_param = f.params.begin(); iter_param != f.params.end(); ++iter_param) {
        if (iter_param->optional == false) ++req;
        if (iter_param->optional == true)  ++opt;
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
        return dtlang::f_quit();
	}

	if (name == "help") {
        if (params.size() == 0) {
            return dtlang::f_help();
        } else if (params.size() == 1) {
            return dtlang::f_help(*(static_cast<std::string*>(params[0].obj)));
        }
	}

	if (name == "set") {
        return dtnet::set(*(static_cast<std::string*>(params[0].obj)), *(static_cast<double*>(params[1].obj)));
	}

    if (name == "version") {
        return dtlang::f_version();
    }

	if (name == "modsim") {
        r = new Simulation((static_cast<Simulation*>(params[0].obj))->net, (static_cast<Simulation*>(params[0].obj))->trial);
        Range new_val(*(static_cast<double*>(params[2].obj)));
        return dtnet::modsim(*(static_cast<Simulation*>(r)), *(static_cast<Simulation*>(params[0].obj)), *(static_cast<std::string*>(params[1].obj)), new_val);
	}

	if (name == "modsimr") {
        r = new Simulation((static_cast<Simulation*>(params[0].obj))->net, (static_cast<Simulation*>(params[0].obj))->trial);
        Range new_val(*(static_cast<double*>(params[2].obj)), *(static_cast<double*>(params[3].obj)), *(static_cast<double*>(params[4].obj)));
        return dtnet::modsim(*(static_cast<Simulation*>(r)), *(static_cast<Simulation*>(params[0].obj)), *(static_cast<std::string*>(params[1].obj)), new_val);
	}

    /*
	if (name == "graphinputs") {
        if (params.size() == 1) {
            return dtlang::f_graphinputs(*(static_cast<Trial*>(params[0].obj)), "inputs.eps");
        } else if (params.size() == 2) {
            return dtlang::f_graphinputs(*(static_cast<Trial*>(params[0].obj)), *(static_cast<std::string*>(params[1].obj)));
        }
	}
    */

	if (name == "graphnetwork") {
        if (params.size() == 1) {
            return dtnet::graphnetwork(*(static_cast<Results*>(params[0].obj)), "network.eps");
        } else if (params.size() == 2) {
            return dtnet::graphnetwork(*(static_cast<Results*>(params[0].obj)), *(static_cast<std::string*>(params[1].obj)));
        }
	}

    if (name == "graphpsth") {
        return dtnet::graphpsth(*(static_cast<Results*>(params[0].obj)),
                                   *(static_cast<std::string*>(params[1].obj)),
                                   *(static_cast<std::string*>(params[2].obj)));
    }

    if (name == "graphtrial_voltage") {
        return dtnet::graphtrial(dtnet::PLOT_VOLTAGE, *(static_cast<Results*>(params[0].obj)),
                                                          (int)*(static_cast<double*>(params[1].obj)),
                                                          *(static_cast<std::string*>(params[2].obj)));
    }
    if (name == "graphtrial_spikes") {
        return dtnet::graphtrial(dtnet::PLOT_SPIKES, *(static_cast<Results*>(params[0].obj)),
                                                         (int)*(static_cast<double*>(params[1].obj)),
                                                         *(static_cast<std::string*>(params[2].obj)));
    }

    if (name == "graphspiketrains") {
        if (params.size() == 2) {
            return dtnet::graphspiketrains(*(static_cast<Results*>(params[0].obj)),
                                              *(static_cast<std::string*>(params[1].obj)),
                                              dtnet::DEFAULT,
                                              dtnet::DEFAULT,
                                              dtnet::DEFAULT,
                                              "spikes.eps");
        } else if (params.size() == 3) {
            return dtnet::graphspiketrains(*(static_cast<Results*>(params[0].obj)),
                                              *(static_cast<std::string*>(params[1].obj)),
                                              dtnet::DEFAULT,
                                              dtnet::DEFAULT,
                                              dtnet::DEFAULT,
                                              *(static_cast<std::string*>(params[2].obj)));

        } else if (params.size() == 4) {
            return dtnet::graphspiketrains(*(static_cast<Results*>(params[0].obj)),
                                              *(static_cast<std::string*>(params[1].obj)),
                                              (int)*(static_cast<double*>(params[3].obj)),
                                              dtnet::DEFAULT,
                                              dtnet::DEFAULT,
                                              *(static_cast<std::string*>(params[2].obj)));
        } else if (params.size() == 5) {
            return dtnet::graphspiketrains(*(static_cast<Results*>(params[0].obj)),
                                              *(static_cast<std::string*>(params[1].obj)),
                                              (int)*(static_cast<double*>(params[3].obj)),
                                              *(static_cast<double*>(params[4].obj)),
                                              dtnet::DEFAULT,
                                              *(static_cast<std::string*>(params[2].obj)));

        } else if (params.size() == 6) {
            return dtnet::graphspiketrains(*(static_cast<Results*>(params[0].obj)),
                                              *(static_cast<std::string*>(params[1].obj)),
                                              (int)*(static_cast<double*>(params[3].obj)),
                                              *(static_cast<double*>(params[4].obj)),
                                              *(static_cast<double*>(params[5].obj)),
                                              *(static_cast<std::string*>(params[2].obj)));
        }
    }

    if (name == "graphspikecounts") {
        return dtnet::graphspikecounts(*(static_cast<Results*>(params[0].obj)),
                                          *(static_cast<std::string*>(params[1].obj)),
                                          *(static_cast<std::string*>(params[2].obj)),
                                          *(static_cast<std::string*>(params[3].obj)),
                                          dtnet::PLOT_FLAT
                                         );
    }
    if (name == "graphspikecounts3d") {
        return dtnet::graphspikecounts(*(static_cast<Results*>(params[0].obj)),
                                          *(static_cast<std::string*>(params[1].obj)),
                                          *(static_cast<std::string*>(params[2].obj)),
                                          *(static_cast<std::string*>(params[3].obj)),
                                          dtnet::PLOT_3D
                                         );
    }
    if (name == "graphspikecountsmap") {
        return dtnet::graphspikecounts(*(static_cast<Results*>(params[0].obj)),
                                          *(static_cast<std::string*>(params[1].obj)),
                                          *(static_cast<std::string*>(params[2].obj)),
                                          *(static_cast<std::string*>(params[3].obj)),
                                          dtnet::PLOT_MAP
                                         );
    }
    if (name == "graphfirstspikelatency") {
        return dtnet::graphfirstspikelatency(*(static_cast<Results*>(params[0].obj)),
                                                *(static_cast<std::string*>(params[1].obj)),
                                                *(static_cast<std::string*>(params[2].obj)),
                                                *(static_cast<std::string*>(params[3].obj)),
                                                 dtnet::PLOT_FLAT
                                               );
    }

	if (name == "vars") {
        return dtlang::f_vars();
	}

	if (name == "funcs") {
        return dtlang::f_funcs();
	}

	if (name == "run") {
		if (r_type == dtlang::NO_RETURN) {
			cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
			return false;
		}
        r = new Results();
        if (params.size() == 3) {
            return dtnet::run( *(static_cast<Results*>(r)),
                                  *(static_cast<Simulation*>(params[0].obj)),
                                  *(static_cast<std::string*>(params[1].obj)),
                                   (int)*(static_cast<double*>(params[2].obj)),
                                   dtnet::get_dbl("delay"),
                                   true // Store voltage by default
                                   );
        } else if (params.size() == 4) {
            return dtnet::run( *(static_cast<Results*>(r)),
                                  *(static_cast<Simulation*>(params[0].obj)),
                                  *(static_cast<std::string*>(params[1].obj)),
                                   (int)*(static_cast<double*>(params[2].obj)),
                                  *(static_cast<double*>(params[3].obj)),
                                   true // Store voltage by default
                                   );
        } else if (params.size() == 5) {
            return dtnet::run( *(static_cast<Results*>(r)),
                                  *(static_cast<Simulation*>(params[0].obj)),
                                  *(static_cast<std::string*>(params[1].obj)),
                                   (int)*(static_cast<double*>(params[2].obj)),
                                  *(static_cast<double*>(params[3].obj)),
                                  *(static_cast<int*>(params[4].obj)) != 0
                                   );
        }
	}

    if (name == "load") {
		if (r_type == dtlang::NO_RETURN) {
			cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
			return false;
		}
        r = new Results();
        return dtnet::load(*(static_cast<Results*>(r)), *(static_cast<std::string*>(params[0].obj)));
    }

	if (name == "print") {
        return dtlang::f_print(params[0].obj, params[0].type);
	}

    if (name == "merge") {
        if (r_type == dtlang::NO_RETURN) {
			cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
			return false;
        }
        r = new Results();
        return dtnet::merge(*(static_cast<Results*>(r)), static_cast<Results*>(params[0].obj), static_cast<Results*>(params[1].obj));
    }

    if (name == "constrain") {
		if (r_type == dtlang::NO_RETURN) {
			cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
			return false;
		}
        r = new Results();
        return dtnet::constrain(*(static_cast<Results*>(r)), static_cast<Results*>(params[0].obj), *(static_cast<std::string*>(params[1].obj)), *(static_cast<double*>(params[2].obj)));
    }

	if (name == "print") {
        return dtlang::f_print(params[0].obj, params[0].type);
	}

	if (name == "external") {
        return dtlang::f_external(*(static_cast<std::string*>(params[0].obj)), end_input);
	}


	if (name == "simulation") {
		if (r_type == dtlang::NO_RETURN) {
			cout << "Error: \"" << name << "\" must be assigned to a variable." << endl;
			return false;
		}

        Net network;
        Trial trial;

        if (dtlang::simulation(*(static_cast<std::string*>(params[0].obj)), *(static_cast<std::string*>(params[1].obj)), &network, &trial)) {
            r = new Simulation(network,trial);
        } else {
            return false;
        }
		return true;
	}

    return false;
}

bool dtlang::f_vars() {

    cout << endl << "Variable" << "\t" << "Type" << "\t" << "Value" << endl;
    cout << "------------------------------------" << endl;
    map<std::string, variable_def, strCmp>::iterator iter;
    for (iter = dtlang::vars.begin(); iter != dtlang::vars.end(); ++iter) {
        cout << iter->first << "\t\t";
        cout << dtlang::type_names[iter->second.type] << "\t";

        switch(iter->second.type) {
            case dtlang::TYPE_STRING:
                cout << *(static_cast<std::string*>(iter->second.obj));
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

            case dtlang::TYPE_RESULTS:
                cout << "[Results Object]";
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
    map<std::string, function_def, strCmp>::iterator iter;
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

bool dtlang::f_help(std::string name) {

    dtlang::function_def f;
    map<std::string, function_def>::iterator iter = dtlang::functions.find(name);

    if (iter == dtlang::functions.end()) {
        cout << "Cannot find a function named \"" << name << "\"" << endl << endl;
        return false;
    }
    f = iter->second;
    vector<function_param>::iterator iter_param;

    cout << endl << name << "(";
    int opt = 0;
    for (iter_param = f.params.begin(); iter_param != f.params.end(); ++iter_param) {
        if (iter_param != f.params.begin()) cout << ", ";
        if (iter_param->optional) { ++opt; cout << '['; }
        cout << "<" << iter_param->name << ":" << dtlang::type_names[iter_param->type] << ">";
    }
    for (int i = 0; i < opt; ++i) { cout << "]"; };
    cout << ")" << endl;
    cout << f.help << endl;
    cout << "Return Type: " << dtlang::type_names[f.return_type] << endl << endl;

    for (iter_param = f.params.begin(); iter_param != f.params.end(); ++iter_param) {
        cout << "<" << iter_param->name << ":" << dtlang::type_names[iter_param->type] << ">";
        if (iter_param->optional) {
            cout << "\t*Optional* (Default: " << iter_param->def << ")" << endl;
        } else {
            cout << "\t[Required]" << endl;
        }
        cout << "\t" << iter_param->help << endl;
    }
    cout << endl;
    return true;
}


bool dtlang::f_quit() {

    // Tell libdtnet that it's time to clean up and go home
    dtnet::quit();

    map<std::string, variable_def>::iterator iter;
    for (iter = vars.begin(); iter != vars.end(); ++iter) {
        dtlang::delete_variable(iter->second);
    }
    for (iter = statements.begin(); iter != statements.end(); ++iter) {
        dtlang::delete_variable(iter->second);
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
            delete static_cast<std::string*>(var.obj);
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

        case dtlang::TYPE_RESULTS:
            delete static_cast<Results*>(var.obj);
            break;

		case dtlang::TYPE_SIMULATION:
			delete static_cast<Simulation*>(var.obj);
			break;

		case dtlang::TYPE_POPULATION:
			delete static_cast<Population*>(var.obj);
			break;

        default:
            cout << "Attempted to free an unknown variable type.  MEMORY LEAK!" << endl;
            return false;
            break;
    }
    return true;
}

bool f_delete(const std::string var) {
   return true;
}

bool dtlang::f_external(const std::string filename, bool &end_input) {

	std::string line;
    ifstream script(filename.c_str());
    if (script.is_open())
    {
        while (!script.eof() && !end_input)
        {
            getline(script, line);
			if (line != "") // Skip blank lines
			{
				cout << VT_set_colors(VT_RED, VT_DEFAULT) << "extern" << VT_default_attributes << "> " << line << endl;
				if (!dtlang::parse(line, end_input))
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

bool dtlang::simulation(const std::string net_filename, const std::string trial_filename, Net *net, Trial *trial) {

	std::string error;
    /* Load Network */
    if (dtlang::verbose) cout << "...Loading network definition from " << net_filename;
    if ( net->load(net_filename, error) == false ) {
        if (dtlang::verbose) cout << "\t[FAILED]" << endl;
		cout << "[X] " << error << endl;
        return false;
    }
    if (dtlang::verbose) cout << "\t[OK]" << endl;
    if (dtlang::verbose) cout << "...Loaded " << net->count_populations() << " populations" << endl;


    /* Load Inputs */
    if (dtlang::verbose) cout << "...Loading input vectors from " << trial_filename;
    if ( trial->load(trial_filename, error) == false ) {
        if (dtlang::verbose) cout << "\t[FAILED]" << endl;
        cout << "[X] " << error << endl;
        return false;
    }
    if (dtlang::verbose) cout << "\t[OK]" << endl;
    if (dtlang::verbose) cout << "...Generated " << trial->count() << " input signals" << endl;

    return true;
}

/*
bool dtlang::f_graphinputs(Trial &trial, std::string const &filename) {
    vector<vector<double> >* signals = trial.signals();
    vector<double>* timesteps = trial.timeSteps();

    GLE gle;
    GLE::PlotProperties plotProperties;
    GLE::Color start;

    start.r = 0.5;
    start.g = 0.5;
    start.b = 0.5;
    plotProperties.first = start;
    plotProperties.zeros = false;
    gle.plot(*timesteps, *signals, plotProperties);

    gle.canvasProperties.width = Settings::instance()->get_dbl("graph.width");
    gle.canvasProperties.height = Settings::instance()->get_dbl("graph.height");
    gle.draw(filename);
    return true;
}
*/

bool dtlang::f_version() {
    cout << "libdtnet: " << dtnet::version() << endl;
    cout << "dtnet CLI: " << DTNETCLI_VERSION << endl;
    return true;
}


bool dtlang::f_print(void* ptr, int const type) {
    switch(type)
    {
        case dtlang::TYPE_STRING:
            cout << *(static_cast<std::string*>(ptr)) << endl;
            break;

        case dtlang::TYPE_TRIAL: cout << static_cast<Trial*>(ptr)->toString();
            break;

        case dtlang::TYPE_NET:
            cout << static_cast<Net*>(ptr)->toString();
            break;

        case dtlang::TYPE_RESULTS:
            cout << static_cast<Results*>(ptr)->toString();
            break;

		case dtlang::TYPE_SIMULATION:
			cout << static_cast<Simulation*>(ptr)->toString();
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

