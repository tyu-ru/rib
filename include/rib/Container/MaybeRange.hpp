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

} // namespace rib
