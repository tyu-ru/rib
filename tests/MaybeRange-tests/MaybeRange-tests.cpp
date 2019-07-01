#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Container/MaybeRange.hpp"

#include "test_util.hpp"

using namespace rib;

using TestTypeList = std::tuple<std::optional<int>, std::shared_ptr<int>,
                                const std::optional<int>, const std::shared_ptr<int>>;

template <class T>
T gen_test_nul();
template <>
std::optional<int> gen_test_nul() { return std::nullopt; }
template <>
std::shared_ptr<int> gen_test_nul() { return nullptr; }
template <class T>
T gen_test_val();
template <>
std::optional<int> gen_test_val() { return 1; }
template <>
std::shared_ptr<int> gen_test_val() { return std::make_unique<int>(1); }

TEMPLATE_LIST_TEST_CASE("MaybeRange - ", "[container]", TestTypeList)
{
    TestType a = gen_test_nul<std::remove_const_t<TestType>>();
    bool f = true;
    for (auto&& x[[maybe_unused]] : MaybeRange(a)) {
        static_assert(std::is_same_v<decltype(x), trait::copy_const_t<TestType, int>&>);
        f = false;
    }
    CHECK(f);

    TestType b = gen_test_val<std::remove_const_t<TestType>>();
    f = false;
    for (auto&& x : MaybeRange(b)) {
        CHECK(x == 1);
        f = true;
    }
    CHECK(f);
}

TEMPLATE_TEST_CASE("MaybeRange - a", "[container]", std::optional<int>, std::shared_ptr<int>)
{
    bool f = true;
    for (auto&& x[[maybe_unused]] : MaybeRange(gen_test_nul<TestType>())) {
        static_assert(std::is_same_v<decltype(x), const int&>);
        f = false;
    }
    CHECK(f);
    f = false;
    for (auto&& x : MaybeRange(gen_test_val<TestType>())) {
        CHECK(x == 1);
        f = true;
    }
    CHECK(f);
}
