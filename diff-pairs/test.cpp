#include <diff_pairs.h>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Your tests here") {
    CHECK(CountPairs({1, 2, 3, 4, 5, 5, 6}, 10) == 2);
    CHECK(CountPairs({1, 2, 5, 5, 5, 5, 6}, 10) == 6);
}
