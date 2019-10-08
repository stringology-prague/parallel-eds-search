#ifndef EDS_TOOLS_SRC_SOPANGMATCHER_H_
#define EDS_TOOLS_SRC_SOPANGMATCHER_H_

#include "Matcher.h"

class SopangMatcher : public Matcher {

public:

    SopangMatcher();

    virtual ~SopangMatcher();

    virtual std::unordered_set<unsigned> Match(const ElasticDegenerateString &eds, const std::string &pattern, const std::string &alphabet) override;

protected:

    /** Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants)
     * from the input file cannot be larger than this value. */
    static constexpr unsigned dBufferSize = 262144;

    /** Buffer size for Shift-Or masks for the input alphabet, must be larger than the largest input character ASCII code,
     * up to 'Z' = 90. */
    static constexpr unsigned maskBufferSize = 91;

    uint64_t *dBuffer{};
    uint64_t maskBuffer[maskBufferSize]{};

    /** Shift-Add counter size in bits. */
    static constexpr unsigned saCounterSize = 5;
    /** Maximum pattern size for approximate search. */
    static constexpr unsigned maxPatternApproxSize = 12;

    /** Full single Shift-Add counter indicating no match. */
    static constexpr uint64_t saFullCounter = 0x10ULL;
    /** Single Shift-Add counter with all bits set. */
    static constexpr uint64_t saCounterAllSet = 0x20ULL - 0x1ULL;

    static constexpr uint64_t allOnes = ~(0x0ULL);

    void fillPatternMaskBuffer(const std::string &pattern, const std::string &alphabet);
};

#endif //EDS_TOOLS_SRC_SOPANGMATCHER_H_
