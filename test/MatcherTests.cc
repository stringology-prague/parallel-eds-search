#include <utility>
#include <fstream>

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <gmock/gmock.h>

#include <SopangMatcher.h>
#include <ElasticDegenerateString.h>

class SopangMatcherTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {
public:
    void SetUp() override {}

    void TearDown() override {}

    SopangMatcher matcher_;

    static std::set<unsigned> ReadResultsFile(const std::string &filename) {
        std::ifstream ifs(filename, std::ios_base::in);
        return std::set<unsigned>((std::istream_iterator<unsigned>(ifs)),std::istream_iterator<unsigned>());
    }

    static ElasticDegenerateString ReadEDSFile(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary);
        ElasticDegenerateString eds;
        ifs >> eds;
        return eds;
    }

    static std::string ReadPatternFile(const std::string &filename) {
        std::ifstream pfs(filename, std::ios::binary);
        pfs.seekg(0, std::ios::end);
        size_t pattern_size = pfs.tellg();
        pfs.seekg(0);

        std::string pattern(pattern_size, 0);
        pfs.read(&pattern[0], pattern_size);
        return pattern;
    }
};

TEST_P(SopangMatcherTest, Match) {
    ElasticDegenerateString eds = ReadEDSFile(std::get<0>(GetParam()));
    ASSERT_TRUE(eds.Size() > 0);
    ASSERT_TRUE(eds.Segments() > 0);

    std::string pattern = ReadPatternFile(std::get<1>(GetParam()));
    ASSERT_FALSE(pattern.empty());

    std::set<unsigned> expected_res = ReadResultsFile(std::get<2>(GetParam()));
    ASSERT_FALSE(expected_res.empty());

    std::unordered_set<unsigned> res = SopangMatcher().Match(eds, pattern, "ACGT");
    std::set<unsigned> sorted_res(std::begin(res), std::end(res));

    ASSERT_EQ(sorted_res, expected_res);
}

std::vector<std::tuple<std::string, std::string, std::string>> values = {
        {"data/chr1337.eds", "data/pattern8.txt", "data/result8.txt"},
        {"data/chr1337.eds", "data/patterns8.txt", "data/results8.txt"},
        {"data/chr1337.eds", "data/patterns16.txt", "data/results16.txt"},
        {"data/chr1337.eds", "data/patterns32.txt", "data/results32.txt"},
        {"data/chr1337.eds", "data/patterns64.txt", "data/results64.txt"}
};

INSTANTIATE_TEST_SUITE_P(SopangMatcherTestSuite, SopangMatcherTest, testing::ValuesIn(values));
