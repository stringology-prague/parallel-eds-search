#include <cassert>

#include "SopangMatcher.h"


void SopangMatcher::fillPatternMaskBuffer(const std::string &pattern, const std::string &alphabet)
{
    assert(pattern.size() > 0 and pattern.size() <= sizeof(uint64_t) * 8);
    assert(alphabet.size() > 0);

    for (const char c : alphabet)
    {
        assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
        maskBuffer[static_cast<unsigned char>(c)] = allOnes;
    }

    for (size_t cid = 0; cid < pattern.size(); ++cid)
    {
        assert(pattern[cid] > 0 and static_cast<unsigned char>(pattern[cid]) < maskBufferSize);
        maskBuffer[static_cast<unsigned char>(pattern[cid])] &= (~(0x1ULL << cid));
    }
}

std::unordered_set<unsigned> SopangMatcher::Match(const ElasticDegenerateString &eds,
                                                    const std::string &pattern,
                                                    const std::string &alphabet) {
    assert(eds.Segments() > 0 and pattern.size() > 0 and pattern.size() <= sizeof(uint64_t) * 8);
    std::unordered_set<unsigned> res;

    fillPatternMaskBuffer(pattern, alphabet);

    const uint64_t hitMask = (0x1ULL << (pattern.size() - 1));
    uint64_t D = allOnes;

    // For all segments
    for (unsigned sid = 0; sid < eds.Segments(); ++sid)
    {
        ElasticDegenerateString::VariantRange segment = eds.GetSegment(sid);
        size_t segmentSize = eds.SegmentSize(sid);

        assert(segmentSize > 0 and segmentSize <= dBufferSize);

        // For all variants in a segment
        for (unsigned vid = 0; vid < segmentSize; ++vid)
        {
            dBuffer[vid] = D;

            ElasticDegenerateString::DataRange variant = eds.GetVariant(segment.first + vid);
            size_t variantSize = eds.VariantSize(segment.first + vid);

            // For all characters in a variant
            for (size_t cid = 0; cid < variantSize; ++cid)
            {
                const char c = static_cast<const char>(variant.first[cid]);

                assert(c > 0 and static_cast<unsigned char>(c) < maskBufferSize);
                assert(alphabet.find(c) != std::string::npos);

                dBuffer[vid] <<= 1;
                dBuffer[vid] |= maskBuffer[static_cast<unsigned char>(c)];

                // Match occurred.
                if ((dBuffer[vid] & hitMask) == 0x0ULL)
                {
                    res.insert(sid);
                }
            }
        }

        D = dBuffer[0];

        for (unsigned vid = 1; vid < segmentSize; ++vid)
        {
            // As a join operation we want to preserve 0s (active states):
            // a match can occur in any segment alternative.
            D &= dBuffer[vid];
        }
    }

    return res;
}
SopangMatcher::SopangMatcher() {
    dBuffer = new uint64_t[dBufferSize];
}

SopangMatcher::~SopangMatcher() {
    delete[] dBuffer;
}

