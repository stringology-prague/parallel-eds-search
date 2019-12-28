#include <fstream>
#include <iterator>
#include <sstream>

#include "Util.h"

std::vector<std::string> eds::util::ReadPatternFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.good()){
        std::stringstream ss;
        ss << "Failed to open pattern file " << filename << "!" << std::endl;
        throw std::logic_error(ss.str());
    }

    std::vector<std::string> patterns;
    for (std::string p; std::getline(ifs, p);)
        patterns.push_back(p);
    return patterns;
}

ElasticDegenerateString eds::util::ReadEDSFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.good()){
        std::stringstream ss;
        ss << "Failed to open EDS file " << filename << "!" << std::endl;
        throw std::logic_error(ss.str());
    }

    ElasticDegenerateString eds;
    ifs >> eds;
    return eds;
}

std::vector<std::set<unsigned>> eds::util::ReadResultsFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.good()){
        std::stringstream ss;
        ss << "Failed to read results file " << filename << "!" << std::endl;
        throw std::logic_error(ss.str());
    }

    std::string line;
    std::vector<std::set<unsigned>> results;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        results.push_back(
                std::set<unsigned>((std::istream_iterator<unsigned>(iss)), std::istream_iterator<unsigned>()));
    }
    return results;
}