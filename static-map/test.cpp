#include <catch.hpp>
#include <util.h>
#include <static_map.h>
#include <ranges>

TEST_CASE("EmptyMap") {
    StaticMap map({});

    std::string value;
    REQUIRE(!map.Find("key", &value));
}

TEST_CASE("SmallMap") {
    StaticMap map({{"b", "1"}, {"a", "2"}});

    std::string value;
    REQUIRE(map.Find("b", &value));
    REQUIRE("1" == value);

    REQUIRE(map.Find("a", &value));
    REQUIRE("2" == value);

    REQUIRE(!map.Find("c", &value));
    REQUIRE(!map.Find("0", &value));
}

TEST_CASE("Speed") {
    constexpr auto kNumElements = 100'000;

    std::vector<std::pair<std::string, std::string>> items;
    for (auto i = 0; i < kNumElements; ++i) {
        items.emplace_back(std::to_string(i), std::to_string(i));
    }

    RandomGenerator rnd(73467345);
    rnd.Shuffle(items.begin(), items.end());

    StaticMap map(items);

    for (auto i = 0; i < kNumElements; ++i) {
        std::string value;
        REQUIRE(map.Find(std::to_string(i), &value));
        REQUIRE(std::to_string(i) == value);
    }
}
