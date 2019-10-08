#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include <random>
#include <iostream>
#include <unordered_set>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <ElasticDegenerateString.h>
#include "SopangMatcher.h"

namespace po = boost::program_options;

enum class ERR
{
    SUCCESS,
    INVALID_ARGS
};

int main(int argc, const char **argv) {

    int seed;
    std::string operation;
    std::vector<std::string> eds_files, patterns;

    po::options_description desc_args("Elastic Degenerate String (EDS) Toolkit\n"
                                      "\n"
                                      "Usage:\n"
                                      "    eds-cli operation [options] [file.eds,...] \n"
                                      "\n"
                                      "Examples:\n"
                                      "    eds-cli match genome1.eds genome2.eds -p pattern.eds\n"
                                      "\n"
                                      "Arguments");
    desc_args.add_options()
        ("help,h", "produce help message")
        ("verbose,v", "print debug information")
        ("pattern,p", po::value<std::vector<std::string>>(&patterns), "EDS file(s) to use as pattern")
        ("srand", po::value<int>(&seed)->default_value(666), "seed for input data generator");

    po::options_description desc_positional("Hidden options");
    desc_positional.add_options()
        ("operation", po::value<std::string>(&operation)->required(), "{details, match}")
        ("eds_files", po::value<std::vector<std::string>>(&eds_files)->required(), "EDS files to process");

    po::options_description cmdline_options;
    cmdline_options.add(desc_args).add(desc_positional);

    po::positional_options_description pdesc;
    pdesc.add("operation", 1);
    pdesc.add("eds_files", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(pdesc).run(), vm);
    try {
        po::notify(vm);
    }
    catch (const po::error &err) {
        std::cout << err.what() << std::endl << std::endl;
        std::cout << desc_args << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc_args << std::endl;
        return 0;
    }

    if (operation == "info")
    {
        if (eds_files.size() == 0)
        {
            std::cerr << "Missing EDS files specified (eds_files)! " << std::endl;
            return static_cast<int>(ERR::INVALID_ARGS);
        }

        std::ifstream ifs (eds_files.at(0), std::ios::binary);
        ElasticDegenerateString eds;
        ifs >> eds;
        std::cout << eds;
    }
    else if (operation == "match")
    {
        if(eds_files.size() == 0)
        {
            std::cerr << "Missing EDS files specified (eds_files)! " << std::endl;
            return static_cast<int>(ERR::INVALID_ARGS);
        }
        if(patterns.size() == 0)
        {
            std::cerr << "No patterns specified for match operation! " << std::endl;
            return static_cast<int>(ERR::INVALID_ARGS);
        }


        std::ifstream ifs (eds_files.at(0), std::ios::binary);
        ElasticDegenerateString eds;
        ifs >> eds;

        std::ifstream pfs(patterns.at(0), std::ios::binary);

        pfs.seekg(0, std::ios::end);
        size_t pattern_size = pfs.tellg();
        pfs.seekg(0);

        std::string pattern(pattern_size, 0);
        pfs.read(&pattern[0], pattern_size);

        std::unordered_set<unsigned> res = SopangMatcher().Match(eds, pattern, "ACGT");

        std::cout << "Match results:";
        for ( auto it = res.begin(); it != res.end(); ++it )
            std::cout << " " << *it;
        std::cout << std::endl;
    }

    return static_cast<int>(ERR::SUCCESS);
}
