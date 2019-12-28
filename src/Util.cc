#include <fstream>
#include <iterator>

#include "Util.h"

std::vector<std::string> eds::util::ReadPatternFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    std::vector<std::string> patterns;
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(patterns));
    return patterns;
}
