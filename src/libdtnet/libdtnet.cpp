#include "libdtnet.h"

Settings *Settings::s_instance = 0;
NeuronFactory *NeuronFactory::s_instance = 0;

boost::threadpool::pool tp(2); // Pool an additional thread for the progress meter


using namespace std;
namespace po = boost::program_options;

string dtnet::version() {
    return LIBDTNET_VERSION;
}

bool dtnet::initialize() {

    po::options_description config("Configuration");
    config.add_options()
            ("models", po::value<string>(), "")
            ;

    char* home = getenv("HOME");
    string config_file(home);
    config_file.append("/.libdtnetrc");
    ifstream ifs(config_file.c_str());

    po::variables_map vm;
    po::store(po::parse_config_file(ifs, config), vm);
    po::notify(vm);

    if (vm.count("models")) {
        istringstream iss(vm["models"].as<string>(), istringstream::in);
        string model_type;
        while (iss >> model_type) {
            NeuronFactory::instance()->loadModel(model_type);
        }
    } else {
        cerr << "Unable to find available models.  Does ~/.libdtnetrc exist?" << endl;
        return false;
    }
	return true;
}

std::list<std::string> dtnet::models() {
    return NeuronFactory::instance()->model_types();
}

std::map<std::string,double> dtnet::defaultModelParams(std::string model_type) {
    Neuron *n;
    NeuronFactory::instance()->create(model_type, NULL, n);
    std::map<std::string,double> r = n->default_parameters();
    return r;
}


bool dtnet::set_threads(int threads) {
    if (threads > 0) {
        tp = boost::threadpool::pool(threads + 1);
        return true;
    }

    return false;
}

bool dtnet::quit() {
    NeuronFactory::instance()->close();
    return true;
}

bool dtnet::set(const string var, double const val) {
    Settings::instance()->set(var, val);
    return true;
}

bool dtnet::set(const string var, string const val) {
    Settings::instance()->set(var, val);
    return true;
}

double dtnet::get_dbl(const std::string var) {
    return Settings::instance()->get_dbl(var);
}

std::string dtnet::get_str(const std::string var) {
    return Settings::instance()->get_str(var);
}

bool dtnet::run(Results &result, Simulation &sim, string filename, int number_of_trials, double delay, bool voltage) {

    double T = Settings::instance()->get_dbl("T");
    double dt = Settings::instance()->get_dbl("dt");

    bool r = sim.run(result, filename, T, dt, delay, number_of_trials, voltage, tp);
    return r;
}

bool dtnet::load(Results &r, const string filename) {
    return Results::load(r, filename);
}

bool dtnet::modsim(Simulation &sim, Simulation &old_sim, const string ID, Range const val) {
    bool r = old_sim.modify(ID, val);
    sim = old_sim;
    return r;
}

bool dtnet::constrain(Results &result, Results *old_results, const string ID, const double value) {
    return old_results->constrain(result, ID, value);
}

bool dtnet::merge(Results &result, Results *r1, Results *r2) {
    return result.merge(*r1, *r2);
}


bool dtnet::graphfirstspikelatency(Results &results, string const &popID, string const &x_axis, string const &filename, int const type) {

    if (results.unconstrained.size() > 2 || results.unconstrained.size() == 0) {
        cout << "[X] Results must be constrained to a one or two parameters." << endl;
        return false;
    }
    if (type != dtnet::PLOT_FLAT && results.unconstrained.size() != 2) {
        cout << "[X] Results must be constrained to two parameters." << endl;
        return false;
    }

    string series = "";
    Range series_range = Range(0);
    map<string, Range>::iterator ucIter;
    for (ucIter = results.unconstrained.begin(); ucIter != results.unconstrained.end(); ++ucIter) {
        if (ucIter->first != x_axis) {
            series = ucIter->first;
            series_range = ucIter->second;
        }
    }

    Results seriesResults;
    GLE gle;
    GLE::PanelID panelID = GLE::NEW_PANEL;
    GLE::PlotProperties plotProperties;
    plotProperties.pointSize = 0.2;
    plotProperties.zeros = false;
    GLE::PanelProperties props;
    double max_value = 0;
    vector< vector<double> > z;
    boost::tuple < vector<double>, vector<double>, vector<double> > values;
    vector<double> means;
    vector<double> err_up;
    vector<double> err_down;

    switch (type) {
        case dtnet::PLOT_FLAT :
            for (Range::iterator sIter = series_range.begin(); sIter != series_range.end(); ++sIter) {

                if (series != "") {
                    results.constrain(seriesResults, series, *sIter);
                } else {
                    seriesResults = results;
                }

                values = seriesResults.firstSpikeLatency(popID, x_axis);
                means = values.get < 0 > ();
                err_up = values.get < 1 > ();
                err_down = values.get < 2 > ();
                panelID = gle.plot(results.unconstrained[x_axis].values, means, err_up, err_down, plotProperties, panelID);
                max_value = max(max_value, *(max_element(means.begin(), means.end())));
            }
            props = gle.getPanelProperties(panelID);
            props.x_title = "";
            props.y_title = "";
            props.title = "";
            props.y_min = 0;
            props.y_max = (int) (max_value + 1); // Use the last y value as the top value.
            props.y_labels = true;
            if (Settings::instance()->get_dbl("graph.legend") == 1) props.legend = true;
            props.x_dsubticks = 1;
            gle.setPanelProperties(props, panelID);
            break;
    }

    gle.canvasProperties.width = Settings::instance()->get_dbl("graph.width");
    gle.canvasProperties.height = Settings::instance()->get_dbl("graph.height");
    gle.draw(filename);

    return true;
}

bool dtnet::graphspikecounts(Results &results, string const &popID, string const &x_axis, string const &filename, int const type) {

    if (results.unconstrained.size() > 2 || results.unconstrained.size() == 0) {
        cout << "[X] Results must be constrained to a one or two parameters." << endl;
        return false;
    }
    if (type != dtnet::PLOT_FLAT && results.unconstrained.size() != 2) {
        cout << "[X] Results must be constrained to two parameters." << endl;
        return false;
    }

    string series = "";
    Range series_range = Range(0);
    map<string, Range>::iterator ucIter;
    for (ucIter = results.unconstrained.begin(); ucIter != results.unconstrained.end(); ++ucIter) {
        if (ucIter->first != x_axis) {
            series = ucIter->first;
            series_range = ucIter->second;
        }
    }

    Results seriesResults;
    GLE gle;
    GLE::PanelID panelID = GLE::NEW_PANEL;
    GLE::PlotProperties plotProperties;
    plotProperties.pointSize = 0.2;
    GLE::PanelProperties props;
    double max_value = 0;
    vector< vector<double> > z;
    boost::tuple < vector<double>, vector<double>, vector<double> > values;
    vector<double> means;
    vector<double> err_up;
    vector<double> err_down;

    switch (type) {
        case dtnet::PLOT_FLAT :
            for (Range::iterator sIter = series_range.begin(); sIter != series_range.end(); ++sIter) {
                if (series != "") {
                    results.constrain(seriesResults, series, *sIter);
                } else {
                    seriesResults = results;
                }

                values = seriesResults.meanSpikeCount(popID, x_axis);
                means = values.get < 0 > ();
                err_up = values.get < 1 > ();
                err_down = values.get < 2 > ();
                panelID = gle.plot(results.unconstrained[x_axis].values, means, err_up, err_down, plotProperties, panelID);
                max_value = max(max_value, *(max_element(means.begin(), means.end())));
            }
            props = gle.getPanelProperties(panelID);
            props.x_title = "";
            props.y_title = "";
            props.title = "";
            props.y_min = 0;
            props.y_max = (int) (max_value + 1); // Use the last y value as the top value.
            props.y_labels = true;
            if (Settings::instance()->get_dbl("graph.legend") == 1) props.legend = true;
            props.x_dsubticks = 1;
            gle.setPanelProperties(props, panelID);
            break;

        case dtnet::PLOT_MAP :
        case dtnet::PLOT_3D :
            for (Range::iterator sIter = series_range.begin(); sIter != series_range.end(); ++sIter) {
                results.constrain(seriesResults, series, *sIter);
                values = seriesResults.meanSpikeCount(popID, x_axis);
                means = values.get < 0 > ();
                z.push_back(means);
            }
            if (type == dtnet::PLOT_MAP) plotProperties.usemap = true;
            else plotProperties.usemap = false;
            panelID = gle.plot3d(results.unconstrained[x_axis].values, results.unconstrained[series].values, z, plotProperties, panelID);
            props = gle.getPanelProperties(panelID);
            props.title = "";
            props.x_title = "";
            props.y_title = "";
            gle.setPanelProperties(props, panelID);
            break;
    }


    gle.canvasProperties.width = Settings::instance()->get_dbl("graph.width");
    gle.canvasProperties.height = Settings::instance()->get_dbl("graph.height");
    gle.draw(filename);

    return true;
}

bool dtnet::graphspiketrains(Results &results, string const &popID, int trials, double start, double end, string const &filename) {

    if (results.unconstrained.size() != 1) {
        cout << "[X] Results must be constrained to a single parameter." << endl;
        return false;
    }

    string param_name = results.unconstrained.begin()->first;
    Range params = results.unconstrained.begin()->second;

    // Produce the truncated timesteps
    vector<double> timesteps;
    vector<double>::iterator iter;

    // Setup timesteps
    int start_index = -9999;
    int end_index = -9999;
    if (start == dtnet::DEFAULT) start = results.timeseries.front();
    if (end == dtnet::DEFAULT) end = results.timeseries.back();
    if (start < results.timeseries.front()) start = results.timeseries.front();
    if (end > results.timeseries.back()) end = results.timeseries.back();
    int count = 0;
    for (iter = results.timeseries.begin(); iter != results.timeseries.end(); ++iter) {
        if (*iter >= start && start_index == -9999) start_index = count;
        if (*iter >= end && end_index == -9999) end_index = count;
        if (*iter >= start && *iter <= end) timesteps.push_back(*iter);
        ++count;
    }
    if (trials == dtnet::DEFAULT) trials = -1; // Ensure we always print all the trials.

    // Get results
    GLE gle;
    GLE::PlotProperties plotProperties;
    plotProperties.zeros = false;
    plotProperties.no_y = true;
    plotProperties.lineWidth = 0;
    plotProperties.pointSize = 0.1;
    plotProperties.marker = "fcircle";

    GLE::PlotProperties sigPlotProperties;
    sigPlotProperties.zeros = false;
    sigPlotProperties.lineWidth = 0;
    sigPlotProperties.marker = "dot";
    sigPlotProperties.pointSize = 0.05;
    GLE::PanelID panelID = GLE::NEW_PANEL;

    vector< vector<double> > signals;
    int trial;
    vector<double> values;
    vector<double> values_raw;
    double y;
    double y_inc = (1 / (1.5 * (double) (*(params.begin() + 1) - *(params.begin())))); // Fill a half of  the verticle area

    vector< pair<double, double> > points;

    for (vector<double>::iterator pIter = params.begin(); pIter != params.end(); ++pIter) {
        plotProperties.y_start = *pIter;
        vector< Results::Result* > r = results.get(param_name, *pIter);
        trial = 0;
        y = *pIter;
        for (vector<Results::Result*>::iterator rIter = r.begin(); rIter != r.end(); ++rIter) {
            Population::ConstrainedPopulation pop = (*rIter)->cNetwork.populations[popID];
            for (vector<double>::iterator nIter = pop.neurons.front()->spikes.begin(); nIter != pop.neurons.front()->spikes.end(); ++nIter) {
                points.push_back(make_pair(*nIter, y));
            }
            ++trial;
            if (trial == trials) break; // Only show up to trials;
            y += y_inc / r.size();
        }
        values_raw = r.front()->cTrial.values;
        values.clear();
        for (int index = start_index; index <= end_index; ++index) {
            // We offset the value by 0.0001 to ensure we never hit EXACTLY on 0.0 and thus not display it.
            if (values_raw[index] != 0) values.push_back(*pIter + 0.0001);
            else values.push_back(0);
        }
        panelID = gle.plot(timesteps, values, sigPlotProperties, panelID);
    }

    panelID = gle.plot(points, plotProperties, panelID);

    GLE::PanelProperties props = gle.getPanelProperties(panelID);
    props.x_title = "";
    props.y_title = "";
    props.title = "";
    props.y_min = params.front();
    props.y_max = max(params.back(), y); // Use the last y value as the top value.
    props.x_min = start;
    props.x_max = end;
    props.y_labels = true;
    props.y_labels_hei = 0.2;
    props.x_labels_hei = 0.25;
    props.x_labels_dist = 0.15;
    props.y_dticks = 1;
    props.x_dsubticks = 1;
    gle.setPanelProperties(props, panelID);

    gle.canvasProperties.width = Settings::instance()->get_dbl("graph.width");
    gle.canvasProperties.height = Settings::instance()->get_dbl("graph.height");
    gle.draw(filename);
    return true;
}

bool dtnet::graphnetwork(Results &results, string const &filename) {
    char data_filename[] = "/tmp/dtnet_dot_XXXXXX";
    int pTemp = mkstemp(data_filename);
    boost::iostreams::file_descriptor_sink sink(pTemp, boost::iostreams::close_handle);
    boost::iostreams::stream<boost::iostreams::file_descriptor_sink> of(sink);
    if (!of) {
        cout << "[X] Unable to create temporary file." << endl;
        return false;
    }
    if (results.get().empty()) {
        cout << "[X] At least one results network must be present." << endl;
        return false;
    }

    Results::Result *r = results.get().at(0);

    of << "digraph G {" << endl;

    // Setup the input box style
    of << "Input [shape=box];" << endl;

    for (map<string, map<string, Net::Connection<double> > >::iterator a = r->cNetwork.connections.begin(); a != r->cNetwork.connections.end(); ++a) {
        for (map<string, Net::Connection<double> >::iterator b = a->second.begin(); b != a->second.end(); ++b) {
            of << b->first << " -> " << a->first << ";" << endl;
        }
        if (r->cNetwork.populations[a->first].accept_input) {
            of << "Input -> " << a->first << ";" << endl;
        }
    }
    of << "}" << endl;

    string command = "dot -Tps " + string(data_filename) + " -o " + filename;
    int ret = system(command.c_str());
    if (ret != 0) {
        cout << "[X] Error running dot." << endl;
    } else {
        cout << "Saved to " << filename << endl;
    }

    remove(data_filename);
    return true;
}

bool dtnet::graphpsth(Results &results, std::string const &popID, std::string const &filename) {

    if (results.unconstrained.size() > 0) {
        cout << "[X] Results must be fully constrained." << endl;
        return false;
    }

    GLE gle;
    GLE::PanelID panelID = GLE::NEW_PANEL;
    GLE::PlotProperties plotProperties;
    plotProperties.pointSize = 0.2;
    GLE::PanelProperties props;
    double max_value = 0;
    vector<double> x;
    vector<double> counts;

    boost::tuple < vector<double>, vector<double> > values;
    values = results.psth(popID, 1.0);
    x = values.get < 0 > ();
    counts = values.get < 1 > ();

    panelID = gle.plot(x, counts, plotProperties, panelID);
    max_value = max(max_value, *(max_element(counts.begin(), counts.end())));
    props = gle.getPanelProperties(panelID);
    props.x_title = "";
    props.y_title = "";
    props.title = "";
    props.y_min = 0;
    props.y_max = (int) (max_value + 1); // Use the last y value as the top value.
    props.y_labels = true;
    if (Settings::instance()->get_dbl("graph.legend") == 1) props.legend = true;
    props.x_dsubticks = 1;
    gle.setPanelProperties(props, panelID);

    gle.canvasProperties.width = Settings::instance()->get_dbl("graph.width");
    gle.canvasProperties.height = Settings::instance()->get_dbl("graph.height");
    gle.draw(filename);

    return true;

}

bool dtnet::graphtrial(int type, Results &results, int trial, string const &filename) {

    vector<Results::Result*> trials = results.get();
    if (trials.empty()) {
        cout << "[X] No results are found in this simulation." << endl;
        return false;
    }
    Results::Result* r = NULL;
    vector<Results::Result*>::iterator i;
    for (i = trials.begin(); i != trials.end(); ++i) {
        if ((*i)->trial_num == trial) {
            r = *i;
            break;
        }
    }
    if (r == NULL) {
        cout << "[X] Requested trial not found.  Please select a trial between 0 and " << (trials.size() - 1) << "." << endl;
        return false;
    }

    vector<double> timesteps = results.timeseries;
    vector< vector<double> > signals;

    GLE gle;
    GLE::PlotProperties plotProperties;
    GLE::Color start;
    GLE::Color end;

    if (type == dtnet::PLOT_VOLTAGE) {
        start.r = 0;
        start.g = 0;
        start.b = 0;
        end.r = 0.25;
        end.g = 0.25;
        end.b = 0.25;
        plotProperties.first = start;
        plotProperties.last = end;
    }

    GLE::PanelID panelID;
    vector<Population::ConstrainedPopulation*>::iterator pop_iter;
    list<Neuron*>::iterator neuron_iter;
    vector<Population::ConstrainedPopulation*> pops = r->cNetwork.popvector();

    for (pop_iter = pops.begin(); pop_iter != pops.end(); ++pop_iter) {
        signals.clear();
        for (neuron_iter = (**pop_iter).neurons.begin(); neuron_iter != (**pop_iter).neurons.end(); ++neuron_iter) {
            switch (type) {
                case dtnet::PLOT_VOLTAGE :
                            signals.push_back((*neuron_iter)->voltage);
                    plotProperties.pointSize = 0;
                    plotProperties.no_y = false;
                    break;
                case dtnet::PLOT_SPIKES :
                            signals.push_back((*neuron_iter)->spikes);
                    plotProperties.no_y = true;
                    plotProperties.pointSize = 0.05;
                    plotProperties.marker = "fcircle";
                    break;
            }
        }
        panelID = gle.plot(timesteps, signals, plotProperties);
        GLE::PanelProperties props = gle.getPanelProperties(panelID);
        switch (type) {
            case dtnet::PLOT_VOLTAGE :
                if (pop_iter == --(pops.end())) props.x_title = "Time (ms)";
                else props.x_title = "";
                props.y_title = "Voltage (mV)";
                props.y_max = 60;
                props.y_min = -100;
                props.y_nticks = 5;
                break;
            case dtnet::PLOT_SPIKES :
                if (pop_iter == --(pops.end())) props.x_title = "Time (ms)";
                else props.x_title = "";
                break;
        }
        props.title = (*pop_iter)->name;
        gle.setPanelProperties(props, panelID);
    }

    gle.canvasProperties.width = Settings::instance()->get_dbl("graph.width");
    gle.canvasProperties.height = Settings::instance()->get_dbl("graph.height");
    gle.draw(filename);
    return true;
}
