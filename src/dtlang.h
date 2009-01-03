
#ifndef DTLANG_H
#define DTLANG_H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include "lib/threadpool/threadpool.hpp"
#include "GLE.h"
#include "net.h"
#include "trial.h"
#include "vt100.h"
#include "simulation.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>


/**
 * dtlang namespace
 */
namespace dtlang 
{
    static const int NO_RETURN = 999999;
    static const int TYPE_ANY = 2948242;
    static const int TYPE_VOID = 1;
    static const int TYPE_STRING = 2;
    static const int TYPE_INT = 3;
    static const int TYPE_DOUBLE = 4;
    static const int TYPE_TRIAL = 5;
    static const int TYPE_NET = 6;
    static const int TYPE_POPULATION = 7;
	static const int TYPE_SIMULATION = 8;

    // Runtime parameters
    static bool verbose;

    using namespace boost::spirit;
    using namespace boost::spirit::ascii;
    using namespace boost::spirit::qi;
    using namespace boost::spirit::arg_names;
    using namespace std;
    namespace phoenix = boost::phoenix;
    namespace fusion = boost::fusion;
    using phoenix::at_c;
    using phoenix::push_back;

    typedef vector<string> parameters;

    struct function_call
    {
        string name;
        string first_param;
        parameters params;
    };

    struct function_param
    {
        int type;
        string help;
        bool optional;
        string def;
    };

    struct function_def
    {
        string help;
        int return_type;
        map<string, function_param> params;
    };

    struct variable_def
    {
        int type;
        void* obj; 
    };

    struct variable_assign
    {
        string name;
        string value;
    };

    extern map<string, function_def> functions;
    extern map<string, variable_def> vars;
    extern map<int, string> type_names;

    /**
     * Function Templates
     */
    void initialize();
    void initialize_functions();
    void initialize_variables();
    bool parse(const string &str, boost::threadpool::pool &tp, bool &end_input);
    bool runFunction(const string &name, const vector<variable_def> &params, boost::threadpool::pool &tp, void *&r, int &r_type, bool &end_input);
    bool parse_statement(const string &str, variable_def &var, const bool assignment, const bool make_copy, boost::threadpool::pool &tp, bool &end_input);
    bool params_to_variables(parameters &params, vector<variable_def> &var_params, boost::threadpool::pool &tp, bool &end_input);
    bool delete_variable(variable_def var);

    bool f_help(string name);
    bool f_help();
    bool f_vars();
    bool f_funcs();
    bool f_quit(boost::threadpool::pool &tp);
    bool f_benchmark(boost::threadpool::pool &tp, double mult);
    bool f_run(Simulation &sim, string filename, int number_of_trials);
    bool f_load(Simulation &sim, const string filename);
    bool f_loadtrial(const string filename, Trial *trial);
    bool f_loadnetwork(const string filename, Net *net);
	bool f_linktrial(Trial &trial, Simulation &sim, const string popID);
    bool f_external(const string filename, boost::threadpool::pool &tp, bool &end_input);
    bool f_print(void* ptr, int const type);

    // Graphing functions
    bool f_graphinputs(Trial &trial, string const &filename);
	bool f_graphnetwork(Simulation &sim, string const &filename);
	bool f_graphtrial_voltage(Simulation &sim, int input, int trial, std::string const &filename);

    /**
     * Comment Parser
     */
    template <typename Iter>
    struct comment_parser : grammar<Iter, space_type>
    {
        comment_parser(): comment_parser::base_type(expr)
        {
            expr        = "#" >> *(print);
        }
        rule<Iter, space_type> expr;
    };

    /**
     * Function Parser
     */
    template <typename Iter>
    struct function_parser : grammar<Iter, function_call(), space_type>
    {
        function_parser(): function_parser::base_type(expr)
        {
            expr        %= name >> "(" >> -param >> *("," > param) >> ")";
            param       %= raw[lexeme[(lit('"') >> *(print - lit('"')) >> lit('"'))] 
                               | lexeme[+(alnum|'_'|'.')] >> -('(' >> -param >> *("," > param)  >> ')')];
            name        %= raw[lexeme[+(alnum|'_'|'.')]];
        }
        rule<Iter, function_call(), space_type> expr;
        rule<Iter, string(), space_type> name;
        rule<Iter, string(), space_type> param;
    };

    /**
     * Assignment Parser
     */
    template <typename Iter>
    struct assignment_parser : grammar<Iter, variable_assign(), space_type>
    {
        assignment_parser(): assignment_parser::base_type(expr)
        {
            expr        %= name >> "=" >> value;
            value       %= raw[lexeme[+(print)]];
            name        %= raw[lexeme[+(alnum|'_')]];
        }
        rule<Iter, variable_assign(), space_type> expr;
        rule<Iter, string(), space_type> name, value;
    };

    /**
     * String Parser
     */
    template <typename Iter>
    struct string_parser : grammar<Iter, string(), space_type>
    {
        string_parser(): string_parser::base_type(expr)
        {
            expr       %= lit('"') >> raw[lexeme[*((print) - lit('"'))]] >> lit('"');
        }
        rule<Iter, string(), space_type> expr;
    };

    /**
     * Double Parser
     */
    template <typename Iter>
    struct double_parser : grammar<Iter, double(), space_type>
    {
        double_parser(): double_parser::base_type(expr)
        {
            expr       %= double_;
        }
        rule<Iter, double(), space_type> expr;
    };

    /**
     * Integer Parser
     */
    template <typename Iter>
    struct integer_parser : grammar<Iter, int(), space_type>
    {
        integer_parser(): integer_parser::base_type(expr)
        {
            expr       %= int_;
        }
        rule<Iter, int(), space_type> expr;
    };

}

BOOST_FUSION_ADAPT_STRUCT(
    dtlang::function_call,
    (std::string, name)
    (std::string, first_param)
    (dtlang::parameters, params)
)

BOOST_FUSION_ADAPT_STRUCT(
    dtlang::variable_assign,
    (std::string, name)
    (std::string, value)
)

#endif
