#include <long_sum.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Your test") {
    CHECK(LongSum("2", "2") == "4");
    CHECK(LongSum("1999", "1") == "2000");
    CHECK(LongSum("9999", "1") == "10000");
}

TEST_CASE("Long string") {
    std::string s1(10'000'000, '9'), s2(1, '1');
    CHECK(LongSum(s1, s2) == "1" + std::string(10'000'000, '0'));
}