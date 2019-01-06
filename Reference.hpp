#pragma once

#include <type_traits>

namespace rib
{

template <class T>
struct Ref
{
  constexpr explicit Ref(T &x) : ref(x) {}

  const T &operator=(const T &) const = delete;
  constexpr T &operator=(const T &x) { return ref = x; }
  constexpr Ref &operator=(const Ref &x) { return ref = x; }

  constexpr operator const T &() const { return ref; }
  constexpr operator T &() { return ref; }

  constexpr const T &get() const { return ref; }
  constexpr T &get() { return ref; }

private:
  T &ref;
};

static_assert(std::is_invocable_v<void(Ref<int>), Ref<int>>);
static_assert(std::is_invocable_v<void(Ref<int>), Ref<int> &>);
static_assert(std::is_invocable_v<void(Ref<int>), Ref<int> &&>);
static_assert(!std::is_invocable_v<void(Ref<int>), int>);
static_assert(!std::is_invocable_v<void(Ref<int>), int &>);
static_assert(!std::is_invocable_v<void(Ref<int>), int &&>);

static_assert(std::is_assignable_v<Ref<int>, int>);
static_assert(std::is_assignable_v<int &, Ref<int>>);
static_assert(std::is_assignable_v<Ref<int>, Ref<int>>);

} // namespace rib
