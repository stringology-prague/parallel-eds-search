#include <utility>
#include <fstream>

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <gmock/gmock.h>

#include <SopangMatcher.h>
#include <ElasticDegenerateString.h>

class SopangMatcherTest : public testing::TestWithParam<std::pair<std::string, std::string>> {
public:
    void SetUp() override {}
    void TearDown() override {}

    SopangMatcher matcher_;
};

// Simple test, does not use gmock
TEST_P(SopangMatcherTest, Match)
{
    std::pair<std::string, std::string> inParam = GetParam();
    std::ifstream ifs(std::get<0>(inParam), std::ios::binary);
    ASSERT_TRUE(ifs.good());
    ElasticDegenerateString eds;
    ifs >> eds;
    ASSERT_TRUE(eds.Size() > 0);
    ASSERT_TRUE(eds.Segments() > 0);

    std::ifstream pfs(std::get<1>(inParam), std::ios::binary);
    ASSERT_TRUE(pfs.good());
    pfs.seekg(0, std::ios::end);
    size_t pattern_size = pfs.tellg();
    pfs.seekg(0);

    std::string pattern(pattern_size, 0);
    pfs.read(&pattern[0], pattern_size);

    std::unordered_set<unsigned> res = SopangMatcher().Match(eds, pattern, "ACGT");
    std::set<unsigned> sorted_res(std::begin(res), std::end(res));

    std::cout << "Match results:";
    for ( auto it = res.begin(); it != res.end(); ++it )
        std::cout << " " << *it;
    std::cout << std::endl;
}

auto values = testing::Values(std::make_pair<std::string,std::string>("data/chr1337.eds", "data/pattern8.txt"));
INSTANTIATE_TEST_SUITE_P(SopangMatcherTestSuite, SopangMatcherTest, values);
