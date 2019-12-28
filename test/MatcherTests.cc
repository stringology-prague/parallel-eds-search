#include <utility>
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <gmock/gmock.h>
#include <boost/range/combine.hpp>

#include <SopangMatcher.h>
#include <ElasticDegenerateString.h>

class SopangMatcherTest
        : public testing::TestWithParam<std::tuple<std::shared_ptr<const ElasticDegenerateString>, std::string, std::set<unsigned>>> {
public:
    void SetUp() override {}

    void TearDown() override {}

    SopangMatcher matcher_;

    static ElasticDegenerateString ReadEDSFile(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary);
        ElasticDegenerateString eds;
        ifs >> eds;
        return eds;
    }

    static std::vector<std::string> ReadPatternFile(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary);
        std::vector<std::string> patterns;
        std::copy(std::istream_iterator<std::string>(ifs),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(patterns));
        return patterns;
    }

    static std::vector<std::set<unsigned>> ReadResultsFile(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary);
        std::string line;
        std::vector<std::set<unsigned>> results;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);
            results.push_back(
                    std::set<unsigned>((std::istream_iterator<unsigned>(iss)), std::istream_iterator<unsigned>()));
        }
        return results;
    }

    static std::vector<std::tuple<std::shared_ptr<const ElasticDegenerateString>, std::string, std::set<unsigned>>>
    GenerateTestSuiteValues() {
        std::vector<std::tuple<std::string, std::string, std::string>> files = {
                {"data/chr1337.eds", "data/pattern8.txt",   "data/result8.txt"},
                {"data/chr1337.eds", "data/patterns8.txt",  "data/results8.txt"},
                {"data/chr1337.eds", "data/patterns16.txt", "data/results16.txt"},
                {"data/chr1337.eds", "data/patterns32.txt", "data/results32.txt"},
                {"data/chr1337.eds", "data/patterns64.txt", "data/results64.txt"}
        };
        std::vector<std::tuple<std::shared_ptr<const ElasticDegenerateString>, std::string, std::set<unsigned>>> cases;
        for (auto fileTuple : files) {
            auto eds = std::make_shared<const ElasticDegenerateString>(ReadEDSFile(std::get<0>(fileTuple)));
            auto patterns = ReadPatternFile(std::get<1>(fileTuple));
            auto results = ReadResultsFile(std::get<2>(fileTuple));
            if (patterns.size() != results.size()) {
                throw std::logic_error("Test initialisation failure - mismatch in patterns and results size");
            }
            for (auto pr : boost::combine(patterns, results)) {
                std::string pattern;
                std::set<unsigned> result;
                boost::tie(pattern, result) = pr;
                cases.push_back({eds, pattern, result});
            }
        }
    }
};

TEST_P(SopangMatcherTest, Match) {
    std::shared_ptr<const ElasticDegenerateString> eds = std::get<0>(GetParam());
    ASSERT_TRUE(eds->Size() > 0);
    ASSERT_TRUE(eds->Segments() > 0);

    std::string pattern = std::get<1>(GetParam());
    ASSERT_FALSE(pattern.empty());

    std::set<unsigned> expected_res = std::get<2>(GetParam());
    ASSERT_FALSE(expected_res.empty());

    std::unordered_set<unsigned> res = SopangMatcher().Match(*eds, pattern, "ACGT");
    std::set<unsigned> sorted_res(std::begin(res), std::end(res));

    ASSERT_EQ(sorted_res, expected_res);
}


INSTANTIATE_TEST_SUITE_P(SopangMatcherTestSuite, SopangMatcherTest,
                         testing::ValuesIn(SopangMatcherTest::GenerateTestSuiteValues()));
