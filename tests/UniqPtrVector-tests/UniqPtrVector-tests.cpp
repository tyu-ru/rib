#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Container/UniqPtrVector.hpp"

TEST_CASE("uniqvec pushback", "[Container]")
{
    rib::UniqPtrVector<int> a;
    a.push_back(std::make_unique<int>(12));
    REQUIRE(a.size() == 1);
    REQUIRE(*a[0] == 12);
}

TEST_CASE("uniqvec comparator", "[Container]")
{
    auto c = rib::UniqPtrVector<int>{}.comparator();
    auto x0 = std::make_unique<int>(0);
    auto x1 = std::make_unique<int>(1);
    auto x2 = std::make_unique<int>(2);
    REQUIRE(c(x0, x1) == true);
    REQUIRE(c(x1, x1) == false);
    REQUIRE(c(x2, x1) == false);
}
