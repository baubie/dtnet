
#ifndef DTLANG_H
#define DTLANG_H

#define DTNETCLI_VERSION "1.9.3"

#include "../libdtnet/libdtnet.h"

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
#include <boost/tuple/tuple.hpp>
#include "vt100.h"
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
    static const int TYPE_RESULTS = 9;

    // Runtime parameters
    static bool verbose;

    using namespace boost::spirit;
    using namespace boost::spirit::ascii;
    using namespace boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    namespace fusion = boost::fusion;

    typedef std::vector<std::string> parameters;

    struct function_call
    {
        std::string name;
        std::string first_param;
        parameters params;
    };

    struct function_param
    {
        int type;
        std::string name;
        std::string help;
        bool optional;
        std::string def;
    };

    struct function_def
    {
        std::string help;
        int return_type;
        std::vector<function_param> params;
    };

    struct variable_def
    {
        int type;
        void* obj;
    };

    struct variable_assign
    {
        std::string name;
        std::string value;
    };

    /**
     * Function Templates
     */
    void initialize();
    void initialize_functions();
    void initialize_variables();
    bool parse(const std::string &str, bool &end_input);
    bool runFunction(const std::string &name, const std::vector<variable_def> &params, void *&r, int &r_type, bool &end_input);
    bool parse_statement(const std::string &str, variable_def &var, const bool assignment, const bool make_copy, bool &end_input);
    bool params_to_variables(parameters &params, std::vector<variable_def> &var_params, bool &end_input);
    bool simulation(const std::string net_filename, const std::string trial_filename, Net *net, Trial *trial);
    bool delete_variable(variable_def var);

    bool f_version();
    bool f_print(void* ptr, int const type);
    bool f_external(const std::string filename, bool &end_input);
    bool f_help(std::string name);
    bool f_help();
    bool f_vars();
    bool f_funcs();
    bool f_delete(const std::string var);
    bool f_quit();

    /**
     * Comment Parser
     */
    template <typename Iter>
    struct comment_parser : grammar<Iter, boost::spirit::ascii::space_type>
    {
        comment_parser(): comment_parser::base_type(expr)
        {
            expr        = "#" >> *(boost::spirit::qi::print);
        }
        rule<Iter, boost::spirit::ascii::space_type> expr;
    };

    /**
     * Function Parser
     */
    template <typename Iter>
    struct function_parser : grammar<Iter, function_call(), boost::spirit::ascii::space_type>
    {
        function_parser(): function_parser::base_type(expr)
        {
            expr        %= name >> "(" >> -param >> *("," > param) >> ")";
            param       %= raw[lexeme[(lit('"') >> *(boost::spirit::qi::print - lit('"')) >> lit('"'))]
                               | lexeme[+(boost::spirit::qi::alnum|'_'|'.'|'-')] >> -('(' >> -param >> *("," > param)  >> ')')];
            name        %= raw[lexeme[+(boost::spirit::qi::alnum|'_'|'.')]];
        }
        rule<Iter, function_call(), boost::spirit::ascii::space_type> expr;
        rule<Iter, std::string(), boost::spirit::ascii::space_type> name;
        rule<Iter, std::string(), boost::spirit::ascii::space_type> param;
    };

    /**
     * Assignment Parser
     */
    template <typename Iter>
    struct assignment_parser : grammar<Iter, variable_assign(), boost::spirit::ascii::space_type>
    {
        assignment_parser(): assignment_parser::base_type(expr)
        {
            expr        %= name >> "=" >> value;
            value       %= raw[lexeme[+(boost::spirit::qi::print)]];
            name        %= raw[lexeme[+(boost::spirit::qi::alnum|'_')]];
        }
        rule<Iter, variable_assign(), boost::spirit::ascii::space_type> expr;
        rule<Iter, std::string(), boost::spirit::ascii::space_type> name, value;
    };

    /**
     * String Parser
     */
    template <typename Iter>
    struct string_parser : grammar<Iter, std::string(), boost::spirit::ascii::space_type>
    {
        string_parser(): string_parser::base_type(expr)
        {
            expr       %= lit('"') >> raw[lexeme[*((boost::spirit::qi::print) - lit('"'))]] >> lit('"');
        }
        rule<Iter, std::string(), boost::spirit::ascii::space_type> expr;
    };

    /**
     * Double Parser
     */
    template <typename Iter>
    struct double_parser : grammar<Iter, double(), boost::spirit::ascii::space_type>
    {
        double_parser(): double_parser::base_type(expr)
        {
            expr       %= double_;
        }
        rule<Iter, double(), boost::spirit::ascii::space_type> expr;
    };

    /**
     * Integer Parser
     */
    template <typename Iter>
    struct integer_parser : grammar<Iter, int(), boost::spirit::ascii::space_type>
    {
        integer_parser(): integer_parser::base_type(expr)
        {
            expr       %= int_;
        }
        rule<Iter, int(), boost::spirit::ascii::space_type> expr;
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
