#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include <random>
#include <iostream>
#include <unordered_set>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <ElasticDegenerateString.h>

namespace po = boost::program_options;

enum class ERR
{
    SUCCESS,
    INVALID_ARGS
};


/** Buffer size for processing segment variants, the size of the largest segment (i.e. the number of variants)
 * from the input file cannot be larger than this value. */
static constexpr unsigned dBufferSize = 262144;

/** Buffer size for Shift-Or masks for the input alphabet, must be larger than the largest input character ASCII code,
 * up to 'Z' = 90. */
static constexpr unsigned maskBufferSize = 91;

uint64_t *dBuffer;
uint64_t maskBuffer[maskBufferSize];

/** Shift-Add counter size in bits. */
static constexpr unsigned saCounterSize = 5;
/** Maximum pattern size for approximate search. */
static constexpr unsigned maxPatternApproxSize = 12;

/** Full single Shift-Add counter indicating no match. */
static constexpr uint64_t saFullCounter = 0x10ULL;
/** Single Shift-Add counter with all bits set. */
static constexpr uint64_t saCounterAllSet = 0x20ULL - 0x1ULL;

static constexpr uint64_t allOnes = ~(0x0ULL);


void fillPatternMaskBuffer(const std::string &pattern, const std::string &alphabet)
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

std::unordered_set<unsigned> match(const ElasticDegenerateString &eds,
                                      const std::string &pattern, const std::string &alphabet)
{

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
            size_t variantSize = eds.VariantSize(vid);

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
        dBuffer = new uint64_t[dBufferSize];

        std::ifstream ifs (eds_files.at(0), std::ios::binary);
        ElasticDegenerateString eds;
        ifs >> eds;

        std::ifstream pfs(patterns.at(0), std::ios::binary);

        pfs.seekg(0, std::ios::end);
        size_t pattern_size = pfs.tellg();
        pfs.seekg(0);

        std::string pattern(pattern_size, 0);
        pfs.read(&pattern[0], pattern_size);

        std::unordered_set<unsigned> res = match(eds, pattern, "ACGT");

        std::cout << "Match results:";
        for ( auto it = res.begin(); it != res.end(); ++it )
            std::cout << " " << *it;
        std::cout << std::endl;

        delete dBuffer;
    }

    return static_cast<int>(ERR::SUCCESS);
}
