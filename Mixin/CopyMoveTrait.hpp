#pragma once

#include <type_traits>

namespace rib::mixin
{

template <bool copy_ctor, bool move_ctor, bool copy_assign, bool move_assign, class Derived>
struct CopyMoveTrait;

#define T true
#define F false
#define DD_T default
#define DD_F delete
#define DECLARE_CM_TRAIT_SPECIALIZED(cctor, mctor, cassign, massign)                      \
    template <class Derived>                                                              \
    struct CopyMoveTrait<cctor, mctor, cassign, massign, Derived>                         \
    {                                                                                     \
        constexpr CopyMoveTrait() noexcept = default;                                     \
        constexpr CopyMoveTrait(const CopyMoveTrait&) noexcept = DD_##cctor;              \
        constexpr CopyMoveTrait(CopyMoveTrait&&) noexcept = DD_##mctor;                   \
        constexpr CopyMoveTrait& operator=(const CopyMoveTrait&) noexcept = DD_##cassign; \
        constexpr CopyMoveTrait& operator=(CopyMoveTrait&&) noexcept = DD_##massign;      \
    };

DECLARE_CM_TRAIT_SPECIALIZED(T, T, T, T)
DECLARE_CM_TRAIT_SPECIALIZED(T, T, T, F)
DECLARE_CM_TRAIT_SPECIALIZED(T, T, F, T)
DECLARE_CM_TRAIT_SPECIALIZED(T, T, F, F)
DECLARE_CM_TRAIT_SPECIALIZED(T, F, T, T)
DECLARE_CM_TRAIT_SPECIALIZED(T, F, T, F)
DECLARE_CM_TRAIT_SPECIALIZED(T, F, F, T)
DECLARE_CM_TRAIT_SPECIALIZED(T, F, F, F)
DECLARE_CM_TRAIT_SPECIALIZED(F, T, T, T)
DECLARE_CM_TRAIT_SPECIALIZED(F, T, T, F)
DECLARE_CM_TRAIT_SPECIALIZED(F, T, F, T)
DECLARE_CM_TRAIT_SPECIALIZED(F, T, F, F)
DECLARE_CM_TRAIT_SPECIALIZED(F, F, T, T)
DECLARE_CM_TRAIT_SPECIALIZED(F, F, T, F)
DECLARE_CM_TRAIT_SPECIALIZED(F, F, F, T)
DECLARE_CM_TRAIT_SPECIALIZED(F, F, F, F)

#undef T
#undef F
#undef DD_true
#undef DD_false
#undef DECLARE_CM_TRAIT_SPECIALIZED

static_assert(std::is_copy_constructible_v<CopyMoveTrait<true, true, true, true, int>>);
static_assert(!std::is_copy_constructible_v<CopyMoveTrait<false, true, true, true, int>>);

static_assert([] {
    struct A : private CopyMoveTrait<false, false, false, false, int>
    {
    };
    return !std::is_copy_constructible_v<A> &&
           !std::is_move_constructible_v<A> &&
           !std::is_copy_assignable_v<A> &&
           !std::is_move_assignable_v<A>;
}());

} // namespace rib::mixin
