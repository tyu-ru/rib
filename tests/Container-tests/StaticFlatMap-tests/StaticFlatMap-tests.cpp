#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Container/StaticFlatMap.hpp"

using namespace rib;

TEST_CASE("StaticFlatMap - initialize")
{
    StaticFlatMap<std::string, int, 4> map;
    REQUIRE(map.size() == 0);
    REQUIRE_FALSE(map.contain("1"));
}

TEST_CASE("StaticFlatMap - insert", "[container]")
{
    StaticFlatMap<std::string, int, 4> map;

    map.insert(std::make_pair("3", 3));
    map.insert(std::make_pair("4", 4));
    map.insert(std::make_pair("1", 1));
    REQUIRE(map.size() == 3);

    SECTION("insert")
    {
        REQUIRE(map.insert(std::make_pair("2", 2)) == std::make_pair(map.begin() + 1, true));
        REQUIRE(map.size() == 4);
        REQUIRE(map.insert(std::make_pair("5", 5)) == std::make_pair(map.end(), false));
        REQUIRE(map.size() == 4);
    }
    SECTION("insert - duplicate")
    {
        REQUIRE(map.insert(std::make_pair("3", 0)) == std::make_pair(map.begin() + 1, false));
    }

    SECTION("contain & at")
    {
        REQUIRE(map.contain("1"));
        REQUIRE(map.at("1") == 1);
        REQUIRE(map.contain("3"));
        REQUIRE(map.at("3") == 3);
        REQUIRE(map.contain("4"));
        REQUIRE(map.at("4") == 4);
    }
    SECTION("iterator")
    {
        auto it = map.begin();
        REQUIRE(it->first == "1");
        REQUIRE(it->second == 1);
        REQUIRE(++it != map.end());
        REQUIRE(it->first == "3");
        REQUIRE(it->second == 3);
        REQUIRE(++it != map.end());
        REQUIRE(it->first == "4");
        REQUIRE(it->second == 4);
        REQUIRE(++it == map.end());
    }
}

TEST_CASE("ConstexprMap", "[container]")
{
    // constexpr auto a = make_ConstexprMap(std::pair{1, "1"}, std::pair{2, "2"});
    // constexpr ConstexprMap b(std::pair(1, "1"), std::pair(2, "2"));
}
