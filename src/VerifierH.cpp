// VerifierH.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <boost/program_options.hpp>

#include "HtmlScanner.h"

#define LOG_FILE_NAME "verifierh.log"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

namespace po = boost::program_options;

void process_program_options(int argc, char* argv[], po::variables_map& vm)
{
    // Declare supported options
    po::options_description visible("Allowed options");
    visible.add_options()
        ("help,h", "produce help message")
        ("version,u", "show version information")
		("tag,t", po::value<string>(), "tag name")
		("attribute,a", po::value<string>(), "attribute name")
		("regex,r", po::value<string>(), "regular expression")
		("insensitive,i", "case insensitive")
        ("log,l", po::value<string>(), "log file")
        ;

	    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("path", po::value<string>(), "initial path")
        ;

    po::positional_options_description positional;
    positional.add("path", -1);

    po::options_description cmdline_options;
    cmdline_options.add(visible).add(hidden);

    try
    {
        po::store(po::command_line_parser(argc, argv).
            options(cmdline_options).positional(positional).run(), vm);
            po::notify(vm);
    }
    catch (std::exception& e)
    {
        cout << e.what() << "\n";
    }

	if(vm.count("help"))
    {
        cout << visible << endl;
        exit(EXIT_SUCCESS);
    }

    if(vm.count("version"))
    {
    	cout << "0.0.1" << endl;
    	exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[])
{
	po::variables_map vm;
	process_program_options(argc, argv, vm);

	if (!vm.count("tag")) {
		cout << "The 'tag' option was not specified" << endl;
        return (EXIT_FAILURE);
	}

	// we do not do any checking for attribute tag because it is optional

	if (!vm.count("regex")) {
		cout << "The 'regex' option was not specified" << endl;
        return (EXIT_FAILURE);
	}

	if (!vm.count("path")) {
		cout << "the 'initial path' option not specified" << endl;
		return (EXIT_FAILURE);
	}

    CHtmlScanner scanner(vm["tag"].as<string>(),
		vm.count("attribute") ? vm["attribute"].as<string>() : "",
		vm["regex"].as<string>(),
		vm["path"].as<string>(),
        vm.count("log") ? vm["log"].as<string>().c_str() : LOG_FILE_NAME,
		vm.count("insensitive")
	);

	if(!scanner.ValidateRegex())
		return -1;

	scanner.Scan();

	cout << endl << "scanned: " << scanner.GetNumScanned()
		<< ", found: " << scanner.GetNumFound()
		<< ", failed to parse: " << scanner.GetNumFailedToParse()
		<<  endl;

	return 0;
}
