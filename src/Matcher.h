//
// Created by lkrcal on 8/10/19.
//

#ifndef EDS_TOOLS_SRC_MATCHER_H_
#define EDS_TOOLS_SRC_MATCHER_H_

#include <unordered_set>

#include "ElasticDegenerateString.h"


class Matcher {

public:

    virtual std::unordered_set<unsigned>
    Match(const ElasticDegenerateString &eds, const std::string &pattern, const std::string &alphabet) = 0;

};

#endif //EDS_TOOLS_SRC_MATCHER_H_
