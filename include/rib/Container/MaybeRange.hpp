#pragma once

#include "../Mixin/MaybeIterable.hpp"
#include "../Mixin/CopyMoveTrait.hpp"

namespace rib
{

template <class T>
struct MaybeRange;

template <class T>
struct MaybeRange<T&>
    : public rib::mixin::MaybeIterable<MaybeRange<T&>, std::decay_t<decltype(*std::declval<T&>())>>,
      private rib::mixin::NonCopyableNonMovable<MaybeRange<T&>>
{
    constexpr MaybeRange() = delete;
    constexpr MaybeRange(T& x) : ref(x) {}

    constexpr operator bool() const { return static_cast<bool>(ref); }
    constexpr decltype(auto) operator*() { return (*ref); }
    constexpr decltype(auto) operator*() const { return (*ref); }

private:
    T& ref;
};
template <class T>
struct MaybeRange<const T&>
    : public rib::mixin::MaybeConstIterable<MaybeRange<const T&>, std::decay_t<decltype(*std::declval<const T&>())>>,
      private rib::mixin::NonCopyableNonMovable<MaybeRange<const T&>>
{
    constexpr MaybeRange() = delete;
    constexpr MaybeRange(const T& x) : ref(x) {}

    constexpr operator bool() const { return static_cast<bool>(ref); }
    constexpr decltype(auto) operator*() const { return (*ref); }

private:
    const T& ref;
};
template <class T>
struct MaybeRange<T&&>
    : public std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(*std::declval<T>())>>,
                                rib::mixin::MaybeConstIterable<MaybeRange<T&&>, std::decay_t<decltype(*std::declval<T>())>>,
                                rib::mixin::MaybeIterable<MaybeRange<T&&>, std::decay_t<decltype(*std::declval<T>())>>>,
      private rib::mixin::NonCopyableNonMovable<MaybeRange<T&&>>
{
    constexpr MaybeRange() = delete;
    constexpr MaybeRange(T&& x) : val(std::move(x)) {}

    constexpr operator bool() { return static_cast<bool>(val); }
    constexpr decltype(auto) operator*() { return (*val); }

private:
    T val;
};

template <class T>
MaybeRange(T&)->MaybeRange<T&>;
template <class T>
MaybeRange(const T&)->MaybeRange<const T&>;
template <class T>
MaybeRange(T &&)->MaybeRange<T&&>;

} // namespace rib
