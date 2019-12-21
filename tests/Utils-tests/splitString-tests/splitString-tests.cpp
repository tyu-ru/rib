#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Utils/splitString.hpp"
#include "rib/Utils/Lambda.hpp"

#include "test_util.hpp"

using namespace rib;

TEST_CASE("splitString", "[utils]")
{
    using VS = std::vector<std::string>;
    using record = std::tuple<std::string, std::string, std::string, VS>;
    auto r = GENERATE(table<std::string, std::string, std::string, VS>({
        record{"empty", "", "", VS{}},
        record{"empty delimiter", "Hello World", "", VS{}},
        record{"no match", "Hello World", ",", VS{"Hello World"}},
        record{"normal1", "Hello World", " ", VS{"Hello", "World"}},
        record{"normal2", "Hello Programming World", " ", VS{"Hello", "Programming", "World"}},
        record{"multi delimiter", "Hello Programming,World", " ,", VS{"Hello", "Programming", "World"}},
    }));

    DYNAMIC_SECTION(std::get<0>(r))
    {
        std::vector<std::string> v;
        splitString(std::get<1>(r), std::get<2>(r), VISITOR_CAP(s, v.emplace_back(s);));
        CHECK(v == std::get<3>(r));
    }
}

TEST_CASE("splitString skip empty", "[util]")
{
    std::vector<std::string> v;
    using VS = std::vector<std::string>;
    auto lmd = VISITOR_CAP(s, v.emplace_back(s););

    SECTION("skip")
    {
        splitString("Hello  Programming World", " ", lmd);
        CHECK(v == VS{"Hello", "Programming", "World"});
    }
    SECTION("keep")
    {
        splitString("Hello  Programming World", " ", lmd, false);
        CHECK(v == VS{"Hello", "", "Programming", "World"});
    }
}
