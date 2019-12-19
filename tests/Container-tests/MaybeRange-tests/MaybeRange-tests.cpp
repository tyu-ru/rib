#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Container/MaybeRange.hpp"

#include "test_util.hpp"

using namespace rib;

using TestTypeList = std::tuple<
    std::optional<int>,
    const std::optional<int>,
    std::shared_ptr<int>,
    const std::shared_ptr<int>,
    std::unique_ptr<int>,
    const std::unique_ptr<int>>;

template <class T>
T gen_test_nul();
template <>
std::optional<int> gen_test_nul() { return std::nullopt; }
template <>
std::shared_ptr<int> gen_test_nul() { return nullptr; }
template <>
std::unique_ptr<int> gen_test_nul() { return nullptr; }
template <class T>
T gen_test_val();
template <>
std::optional<int> gen_test_val() { return 1; }
template <>
std::shared_ptr<int> gen_test_val() { return std::make_unique<int>(1); }
template <>
std::unique_ptr<int> gen_test_val() { return std::make_unique<int>(1); }

TEMPLATE_LIST_TEST_CASE("MaybeRange_lvalue", "[container]", TestTypeList)
{
    SECTION("null")
    {
        TestType a = gen_test_nul<std::remove_const_t<TestType>>();
        for (auto&& x[[maybe_unused]] : MaybeRange(a)) {
            FAIL();
        }
    }
    SECTION("has value")
    {
        TestType b = gen_test_val<std::remove_const_t<TestType>>();
        bool f = false;
        for (auto&& x : MaybeRange(b)) {
            STATIC_REQUIRE(std::is_same_v<decltype(x), trait::copy_const_t<TestType, int>&>);
            CHECK(x == 1);
            f = true;
            if constexpr (!std::is_const_v<TestType>) {
                x = 2;
            }
        }
        CHECK(f);

        if constexpr (!std::is_const_v<TestType>) {
            CHECK(*b == 2);
        }
    }
}

TEMPLATE_TEST_CASE("MaybeRange_rvalue", "[container]", std::optional<int>, std::shared_ptr<int>, std::unique_ptr<int>)
{
    SECTION("null")
    {
        for (auto&& x[[maybe_unused]] : MaybeRange(gen_test_nul<TestType>())) {
            FAIL();
        }
    }
    SECTION("has value")
    {
        bool f = false;
        for (auto&& x : MaybeRange(gen_test_val<TestType>())) {
            STATIC_REQUIRE(std::is_same_v<decltype(x), int&>);
            CHECK(x == 1);
            f = true;
        }
        CHECK(f);
    }
}

TEST_CASE("MaybeRange_shared", "[container]")
{
    auto a = std::make_shared<int>(1);
    auto b = a;
    for (auto&& x : MaybeRange(std::move(b))) {
        CHECK(x == 1);
        x = 2;
    }
    CHECK(*a == 2);
}
