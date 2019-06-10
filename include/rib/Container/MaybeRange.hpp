#pragma once

#include "../Iterator/MaybeIterator.hpp"
#include "../Mixin/CopyMoveTrait.hpp"

namespace rib
{

template <class T>
struct MaybeRange
    : public rib::MaybeIteratible<MaybeRange<T>, std::decay_t<decltype(*std::declval<T&>())>>,
      private rib::mixin::CopyMoveTrait<false, false, false, false, MaybeRange<T>>
{
    constexpr MaybeRange(T x) : payload(x) {}

    constexpr operator bool() const { return static_cast<bool>(payload); }
    constexpr decltype(auto) operator*() { return (*payload); }
    constexpr decltype(auto) operator*() const { return (*payload); }

private:
    T payload;
};
template <class T>
struct MaybeRange<const T&>
    : public rib::MaybeConstIteratible<MaybeRange<const T&>, std::decay_t<decltype(*std::declval<const T&>())>>,
      private rib::mixin::CopyMoveTrait<false, false, false, false, MaybeRange<const T&>>
{
    constexpr MaybeRange(T x) : payload(x) {}

    constexpr operator bool() const { return static_cast<bool>(payload); }
    constexpr decltype(auto) operator*() const { return (*payload); }

private:
    T payload;
};

template <class T>
MaybeRange(T&)->MaybeRange<T&>;
template <class T>
MaybeRange(const T&)->MaybeRange<const T&>;

static_assert([] {
    std::optional<int> a;
    for (auto&& x[[maybe_unused]] : MaybeRange(a)) {
        static_assert(std::is_same_v<decltype(x), int&>);
        return false;
    }
    a = 1;
    int e = 0;
    for (auto&& x : MaybeRange(a)) {
        e = x;
        x = 2;
    }
    return e == 1 && a == 2;
}());
static_assert([] {
    const std::optional<int> a;
    for (auto&& x[[maybe_unused]] : MaybeRange(a)) {
        static_assert(std::is_same_v<decltype(x), const int&>);
        return false;
    }
    const std::optional<int> b = 1;
    for (auto&& x : MaybeRange(b)) {
        return x == 1;
    }
    return false;
}());
static_assert([] {
    for (auto&& x[[maybe_unused]] : MaybeRange(std::optional<int>(std::nullopt))) {
        static_assert(std::is_same_v<decltype(x), const int&>);
        return false;
    }
    for (auto&& x : MaybeRange(std::optional<int>(1))) {
        return x == 1;
    }
    return false;
}());

} // namespace rib
