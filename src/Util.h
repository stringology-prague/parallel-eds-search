#ifndef EDS_TOOLS_UTIL_H
#define EDS_TOOLS_UTIL_H

#include <set>
#include <string>
#include <vector>

#include <ElasticDegenerateString.h>

namespace eds::util {
    std::vector<std::string> ReadPatternFile(const std::string &filename);
    std::vector<std::set<unsigned>> ReadResultsFile(const std::string &filename);
    ElasticDegenerateString ReadEDSFile(const std::string &filename);
};


#endif //EDS_TOOLS_UTIL_H
