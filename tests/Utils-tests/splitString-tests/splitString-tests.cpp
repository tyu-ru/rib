#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Utils/splitString.hpp"
#include "rib/Utils/Lambda.hpp"

#include "test_util.hpp"

using namespace rib;

TEST_CASE("splitString", "[utils]")
{
    std::vector<std::string> v, e;
    auto lmd = VISITOR_CAP(s, v.emplace_back(s););
    SECTION("empty string")
    {
        splitString("", "", lmd);
        e.assign({});
    }
    SECTION("normal1")
    {
        splitString("Hello World", " ", lmd);
        e.assign({"Hello", "World"});
    }
    SECTION("normal2")
    {
        splitString("Hello Programming World", " ", lmd);
        e.assign({"Hello", "Programming", "World"});
    }
    SECTION("empty delimiter")
    {
        splitString("Hello Programming World", "", lmd);
        e.assign({});
    }
    SECTION("skip empty")
    {
        SECTION("skip")
        {
            splitString("Hello  Programming World", " ", lmd);
            e.assign({"Hello", "Programming", "World"});
        }
        SECTION("keep")
        {
            splitString("Hello  Programming World", " ", lmd, false);
            e.assign({"Hello", "", "Programming", "World"});
        }
    }
    SECTION("multi delimiter")
    {
        splitString("Hello Programming,World", " ,", lmd);
        e.assign({"Hello", "Programming", "World"});
    }
    CHECK(v == e);
}
