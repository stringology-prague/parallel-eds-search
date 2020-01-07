#include <array>
#include <cassert>
#include <stdexcept>
#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <ammintrin.h>
#include <limits>

#include "SSEMatcher.h"

__m128i SetBitN(__m128i value, int N)
{
    __m128i allzero = _mm_setzero_si128();
    __m128i allones = _mm_cmpeq_epi32(allzero, allzero);
    __m128i lowones = _mm_srli_epi64(allones, 63);
    __m128i one = N < 64 ? _mm_srli_si128(lowones, 64 / 8) : _mm_slli_si128(lowones, 64 / 8);
    return _mm_or_si128(value, _mm_slli_epi64(one, N & 63));
}
__m128i ClearBitN(__m128i value, int N)
{
    __m128i allzero = _mm_setzero_si128();
    __m128i allones = _mm_cmpeq_epi32(allzero, allzero);
    __m128i lowones = _mm_srli_epi64(allones, 63);
    __m128i one = N < 64 ? _mm_srli_si128(lowones, 64 / 8) : _mm_slli_si128(lowones, 64 / 8);
    return _mm_andnot_si128(_mm_slli_epi64(one, N & 63), value);
}
bool TestBitN(__m128i value, int N)
{
    __m128i positioned = _mm_slli_epi64(value, 7 - (N & 7));
    return (_mm_movemask_epi8(positioned) & (1 << (N / 8))) != 0;
}

eds::match::SSEMatcher::SSEMatcher(const std::string &alphabet) {
    if (alphabet.empty()) {
        throw std::logic_error("Cannot create SSEMatcher with empty alphabet!");
    }

    alphabet_mask.fill(_mm_setzero_si128());

    for (const char c : alphabet) {
        alphabet_mask[static_cast<unsigned char>(c)] = _mm_set1_epi32(0xFFFFFFFF);
    }
}

std::unordered_set<unsigned>
eds::match::SSEMatcher::Match(const ElasticDegenerateString &eds, const std::string &pattern) {

    std::array<__m128i,static_cast<unsigned>(std::numeric_limits<unsigned char>::max())> pattern_mask = alphabet_mask;

    // Preprocess pattern mask
    for (unsigned pos = 0; pos < pattern.size(); pos++) {
        const char c = pattern.at(pos);
        pattern_mask[static_cast<unsigned char>(c)] = ClearBitN(pattern_mask[static_cast<unsigned char>(c)],pos);
    }

    std::unordered_set<unsigned> res;

    const __m128i hit_mask = SetBitN(_mm_setzero_si128(),pattern.size() - 1);
    __m128i match_mask = _mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128());

    // For all segments
    for (unsigned sid = 0; sid < eds.Segments(); ++sid)
    {
        ElasticDegenerateString::VariantRange segment = eds.GetSegment(sid);

        size_t segmentSize = eds.SegmentSize(sid);
        std::vector<__m128i> variant_match_mask(segmentSize);

        // For all variants in a segment
        for (unsigned vid = 0; vid < segmentSize; ++vid)
        {
            variant_match_mask[vid] = match_mask;

            ElasticDegenerateString::DataRange variant = eds.GetVariant(segment.first + vid);
            size_t variantSize = eds.VariantSize(segment.first + vid);

            // For all characters in a variant
            for (size_t cid = 0; cid < variantSize; ++cid)
            {
                const char c = static_cast<const char>(variant.first[cid]);

                // Shift variant_match_mask[vid] left by 1, then OR with pattern_mask[c]
                __m128i vmm = variant_match_mask[vid];
                variant_match_mask[vid] =
                        _mm_or_si128( _mm_or_si128(_mm_slli_epi64(vmm, 1),
                                _mm_srli_epi64(_mm_slli_si128(vmm, 8), 64-1)),
                                        pattern_mask[static_cast<unsigned char>(c)]);
                // Match occurred.
                if (_mm_testz_si128(variant_match_mask[vid],hit_mask))
                {
                    res.insert(sid);
                }
            }
        }

        // As a join operation we want to preserve 0s (active states):
        // a match can occur in any segment alternative.
        match_mask = variant_match_mask[0];
        for (unsigned vid = 1; vid < segmentSize; ++vid)
        {
            match_mask = _mm_and_si128(match_mask,variant_match_mask[vid]);
        }
    }

    return res;
}

std::unordered_set<unsigned>
eds::match::SSEMatcher::Match(const ElasticDegenerateString &eds, const std::string &pattern, const std::string &alphabet) {
    return std::unordered_set<unsigned int>();
}

