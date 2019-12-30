#ifndef EDS_TOOLS_SSEMATCHER_H
#define EDS_TOOLS_SSEMATCHER_H

#include "Matcher.h"

namespace eds {
namespace match {
    class SSEMatcher : public Matcher {
    public:
        SSEMatcher(const std::string &alphabet);

        virtual std::unordered_set<unsigned> Match(const ElasticDegenerateString &eds, const std::string &pattern, const std::string &alphabet) override;

    protected:
        std::array<__m128i, sizeof(char) * 8> alphabet_mask;
    };
}
}

#endif //EDS_TOOLS_SSEMATCHER_H
