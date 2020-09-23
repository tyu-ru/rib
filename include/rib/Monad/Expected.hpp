#pragma once

#include <variant>
#include <optional>
#include <functional>

#include "../Traits/TypeTraits.hpp"
#include "../Traits/FuncTraits.hpp"
#include "../Mixin/CopyMoveTrait.hpp"

namespace rib
{

struct BadExpectedAccess
{
    const char* what() const noexcept
    {
        return "bad Expected<T, E> access";
    }
};

template <class T>
class Expect
{
    template <class, class>
    friend class Expected;
    T payload;

public:
    constexpr Expect(const T& val) : payload(val) {}
    constexpr Expect(T&& val) : payload(std::move(val)) {}

    template <class... Args>
    constexpr Expect(Args&&... args) : payload(std::forward<Args>(args)...) {}

    constexpr const T& value() const& { return payload; }
};

/**
 * @brief indicate unexpect value
 * @tparam E
 */
template <class E>
class Unexpect
{
    template <class, class>
    friend class Expected;
    E payload;

public:
    constexpr Unexpect(const E& e) : payload(e) {}
    constexpr Unexpect(E&& e) : payload(std::move(e)) {}

    template <class... Args>
    constexpr Unexpect(Args&&... args) : payload(std::forward<Args>(args)...) {}

    constexpr const E& value() const& { return payload; }
};
Unexpect(const char[])->Unexpect<const char*>;

template <class T, class E>
class Expected;

struct ExpectTag
{
    explicit ExpectTag() = default;
};
inline constexpr ExpectTag expect_tag_v{};
struct UnexpectTag
{
    explicit UnexpectTag() = default;
};
inline constexpr UnexpectTag unexpect_tag_v{};

namespace expected_details
{

template <class, class>
struct is_same_value_expected : std::false_type
{
};
template <class T, class E, class F>
struct is_same_value_expected<Expected<T, E>, Expected<T, F>> : std::true_type
{
};
template <class T, class U>
static constexpr bool is_same_value_expected_v = is_same_value_expected<T, U>::value;

template <class, class>
struct is_same_error_expected : std::false_type
{
};
template <class T, class U, class E>
struct is_same_error_expected<Expected<T, E>, Expected<U, E>> : std::true_type
{
};
template <class T, class U>
static constexpr bool is_same_error_expected_v = is_same_error_expected<T, U>::value;

} // namespace expected_details

/**
 * @brief Expected value
 * like `Result` in `rust`
 * @tparam T Ok (require destructible)
 * @tparam E Err (require destructible)
 */
template <class T, class E>
class[[nodiscard]] Expected : mixin::CopyMoveTraitInherit<Expected<T, E>, T, E>
{
    struct Ok
    {
        T v;
        template <class U>
        constexpr Ok(U&& ok) : v(std::forward<U>(ok)) {}
    };
    struct Err
    {
        E v;
        template <class F>
        constexpr Err(F&& err) : v(std::forward<F>(err)) {}
    };

    std::variant<Ok, Err> payload;

public:
    using OkType = T;
    using ErrType = E;

public:
    /// [deleted] default constructor
    constexpr Expected() = delete;
    /// copy constructor
    constexpr Expected(const Expected&) noexcept(std::is_nothrow_copy_constructible_v<decltype(payload)>) = default;
    /// move constructor
    constexpr Expected(Expected &&) noexcept(std::is_nothrow_move_constructible_v<decltype(payload)>) = default;

    /// construct by ok value
    template <class... Args, trait::concept_t<std::is_constructible_v<T, Args&&...>> = nullptr>
    constexpr Expected(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
        : payload(std::in_place_type<Ok>, std::forward<Args>(args)...) {}

    /// construct by ok value
    template <class U, trait::concept_t<std::is_constructible_v<T, U>> = nullptr>
    constexpr Expected(const Expect<U>& expect) noexcept(std::is_nothrow_constructible_v<T, U>)
        : payload(std::in_place_type<Ok>, expect.payload) {}
    /// construct by ok value
    template <class U, trait::concept_t<std::is_constructible_v<T, U>> = nullptr>
    constexpr Expected(Expect<U> && expect) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : payload(std::in_place_type<Ok>, std::move(expect.payload)) {}
    /// construct by ok value
    template <class... Args, trait::concept_t<std::is_constructible_v<T, Args&&...>> = nullptr>
    constexpr Expected(ExpectTag, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
        : payload(std::in_place_type<Ok>, std::forward<Args>(args)...) {}

    /// construct by err value
    template <class F, trait::concept_t<std::is_constructible_v<E, F>> = nullptr>
    constexpr Expected(const Unexpect<F>& err) noexcept(std::is_nothrow_constructible_v<E, F>)
        : payload(std::in_place_type<Err>, err.payload) {}
    /// construct by err value
    template <class F, trait::concept_t<std::is_constructible_v<E, F>> = nullptr>
    constexpr Expected(Unexpect<F> && err) noexcept(std::is_nothrow_constructible_v<E, F&&>)
        : payload(std::in_place_type<Err>, std::move(err.payload)) {}
    /// construct by err value
    template <class... Args, trait::concept_t<std::is_constructible_v<E, Args&&...>> = nullptr>
    constexpr Expected(UnexpectTag, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
        : payload(std::in_place_type<Err>, std::forward<Args>(args)...) {}

    /// copy assign
    constexpr Expected& operator=(const Expected&) = default;
    /// move assign
    constexpr Expected& operator=(Expected&&) = default;

    /// has ok value
    constexpr bool valid() const noexcept { return std::holds_alternative<Ok>(payload); }
    /// has ok value
    constexpr operator bool() const noexcept { return valid(); }

    /// value (no exception)
    constexpr const T& value_noexcept() const& noexcept { return std::get<Ok>(payload).v; }
    /// value (no exception)
    constexpr T& value_noexcept() & noexcept { return std::get<Ok>(payload).v; }
    /// value (no exception)
    constexpr T value_noexcept() && noexcept { return std::move(std::get<Ok>(payload).v); }

    /// value (exception)
    constexpr const T& value() const& noexcept(false)
    {
        if (!valid()) throw BadExpectedAccess{};
        return value_noexcept();
    }
    /// value (exception)
    constexpr T& value() & noexcept(false)
    {
        if (!valid()) throw BadExpectedAccess{};
        return value_noexcept();
    }
    /// value (exception)
    constexpr T value() && noexcept(false)
    {
        if (!valid()) throw BadExpectedAccess{};
        return std::move(value_noexcept());
    }

    /// value (no exception)
    constexpr const T& operator*() const& noexcept { return value_noexcept(); }
    /// value (no exception)
    constexpr T& operator*() & noexcept { return value_noexcept(); }
    /// value (no exception)
    constexpr T operator*() && noexcept { return std::move(value_noexcept()); }

    /// if `*this` has value returns contained value, otherwise returns default_value
    template <class U>
    constexpr T value_or(U && default_value) const&
    {
        if (valid()) return value_noexcept();
        return static_cast<T>(std::forward<U>(default_value));
    }
    /// if *this has value returns contained value, otherwise returns default_value
    template <class U>
    constexpr T value_or(U && default_value)&&
    {
        if (valid()) return std::move(value_noexcept());
        return static_cast<T>(std::forward<U>(default_value));
    }

    /// if `* this` has value returns contained value, otherwise returns default construction
    template <class U = T, trait::concept_t<std::is_same_v<T, U> && (std::is_default_constructible_v<T> || std::is_aggregate_v<T>)> = nullptr>
    constexpr T value_or_default() const&
    {
        if (valid()) return value_noexcept();
        if constexpr (std::is_aggregate_v<T>) {
            return T{};
        } else {
            return T();
        }
    }
    /// if `* this` has value returns contained value, otherwise returns default construction
    template <class U = T, trait::concept_t<std::is_same_v<T, U> && (std::is_default_constructible_v<T> || std::is_aggregate_v<T>)> = nullptr>
    constexpr T value_or_default()&&
    {
        if (valid()) return std::move(value_noexcept());
        if constexpr (std::is_aggregate_v<T>) {
            return T{};
        } else {
            return T();
        }
    }

    template <class F>
    constexpr T value_or_else(F && f) const&
    {
        if (valid()) return value_noexcept();
        if constexpr (std::is_invocable_r_v<T, F, E>) {
            return trait::invoke_constexpr(std::forward<F>(f), error_noexcept());
        } else if constexpr (std::is_invocable_r_v<T, F>) {
            return trait::invoke_constexpr(std::forward<F>(f));
        } else {
            static_assert([] { return false; });
        }
    }
    template <class F>
    constexpr T value_or_else(F && f)&&
    {
        if (valid()) return std::move(value_noexcept());
        if constexpr (std::is_invocable_r_v<T, F, E>) {
            return trait::invoke_constexpr(std::forward<F>(f), std::move(error_noexcept()));
        } else if constexpr (std::is_invocable_r_v<T, F>) {
            return trait::invoke_constexpr(std::forward<F>(f));
        } else {
            static_assert([] { return false; });
        }
    }

    /// pointer (no exception)
    constexpr const T* operator->() const noexcept { return &value_noexcept(); }
    /// pointer (no exception)
    constexpr T* operator->() noexcept { return &value_noexcept(); }

    /// error value (no exception)
    constexpr const E& error_noexcept() const& noexcept { return std::get<Err>(payload).v; }
    /// error value (no exception)
    constexpr E& error_noexcept() & noexcept { return std::get<Err>(payload).v; }
    /// error value (no exception)
    constexpr E error_noexcept() && noexcept { return std::move(std::get<Err>(payload).v); }

    /// error value (exception)
    constexpr const E& error() const& noexcept(false)
    {
        if (valid()) throw BadExpectedAccess{};
        return error_noexcept();
    }
    /// error value (exception)
    constexpr E& error() & noexcept(false)
    {
        if (valid()) throw BadExpectedAccess{};
        return error_noexcept();
    }
    /// error value (exception)
    constexpr E error() && noexcept(false)
    {
        if (valid()) throw BadExpectedAccess{};
        return std::move(error_noexcept());
    }

    /// regenerate Unexpect
    constexpr Unexpect<E> unexpected() const& noexcept(false) { return error(); }
    /// regenerate Unexpect
    constexpr Unexpect<E> unexpected() && noexcept(false) { return std::move(error()); }

    /// optional<T>
    constexpr std::optional<T> optional() const& noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        if (valid()) return value_noexcept();
        return std::nullopt;
    }
    /// optional<T>
    constexpr std::optional<T> optional() && noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (valid()) return std::move(value_noexcept());
        return std::nullopt;
    }
    /// optional<E>
    constexpr std::optional<E> optional_err() const& noexcept(std::is_nothrow_copy_constructible_v<E>)
    {
        if (!valid()) return error_noexcept();
        return std::nullopt;
    }
    /// optional<E>
    constexpr std::optional<E> optional_err() && noexcept(std::is_nothrow_move_constructible_v<E>)
    {
        if (!valid()) return std::move(error_noexcept());
        return std::nullopt;
    }

    /// equally compare
    friend constexpr bool operator==(const Expected& lhs, const Expected& rhs) noexcept
    {
        if (lhs && rhs) return *lhs == *rhs;
        if (!lhs && !rhs) return lhs.error() == rhs.error();
        return false;
    }
    /// unequally compare
    friend constexpr bool operator!=(const Expected& lhs, const Expected& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    /// equally compare
    template <class U>
    friend constexpr bool operator==(const Expected& lhs, const U& rhs) noexcept
    {
        if (lhs) return *lhs == rhs;
        return false;
    }
    /// unequally compare
    template <class U>
    friend constexpr bool operator!=(const Expected& lhs, const U& rhs) noexcept
    {
        return !(lhs == rhs);
    }
    /// equally compare
    template <class U>
    friend constexpr bool operator==(const U& lhs, const Expected& rhs) noexcept
    {
        if (rhs) return lhs == *rhs;
        return false;
    }
    /// unequally compare
    template <class U>
    friend constexpr bool operator!=(const U& lhs, const Expected& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    /// equally compare
    template <class X>
    friend constexpr bool operator==(const Expected& lhs, const Unexpect<X>& rhs) noexcept
    {
        if (!lhs) return lhs.error() == rhs.value();
        return false;
    }
    /// unequally compare
    template <class X>
    friend constexpr bool operator!=(const Expected& lhs, const Unexpect<X>& rhs) noexcept
    {
        return !(lhs == rhs);
    }
    /// equally compare
    template <class X>
    friend constexpr bool operator==(const Unexpect<X>& lhs, const Expected& rhs) noexcept
    {
        if (!rhs) return lhs.value() == rhs.error();
        return false;
    }
    /// unequally compare
    template <class X>
    friend constexpr bool operator!=(const Unexpect<X>& lhs, const Expected& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    /// swap value
    void swap(Expected & rhs) noexcept(std::is_nothrow_swappable_v<decltype(payload)>)
    {
        payload.swap(rhs.payload);
    }

    /**
     * @brief map
     * @details Calls op if the expected is valid, otherwise returns the 'error()' value of self.
     * @param op T -> U
     * @return Expected<U, E>
     */
    template <class Op>
    constexpr auto map(Op && op) const&->Expected<std::invoke_result_t<Op, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), value_noexcept());
    }
    template <class Op>
    constexpr auto map(Op && op)&->Expected<std::invoke_result_t<Op, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), value_noexcept());
    }
    template <class Op>
    constexpr auto map(Op && op)&&->Expected<std::invoke_result_t<Op, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), std::move(value_noexcept()));
    }

    /**
     * @brief emap
     * @details Calls op if the expected is invalid, otherwise returns the 'value()' value of self.
     * @param op E -> F
     * @return Expected<T, F>
     */
    template <class Op>
    constexpr auto emap(Op && op) const&->Expected<T, std::invoke_result_t<Op, E>>
    {
        if (valid()) return value_noexcept();
        return {unexpect_tag_v, trait::invoke_constexpr(std::forward<Op>(op), error())};
    }
    template <class Op>
    constexpr auto emap(Op && op)&->Expected<T, std::invoke_result_t<Op, E>>
    {
        if (valid()) return value_noexcept();
        return {unexpect_tag_v, trait::invoke_constexpr(std::forward<Op>(op), error())};
    }
    template <class Op>
    constexpr auto emap(Op && op)&&->Expected<T, std::invoke_result_t<Op, E>>
    {
        if (valid()) return std::move(value_noexcept());
        return {unexpect_tag_v, trait::invoke_constexpr(std::forward<Op>(op), std::move(error()))};
    }

    /**
     * @brief and_then
     * @details Calls op if the expected is valid, otherwise returns the 'error()' value of self.
     * @param op T -> Expected<U, E>
     * @return Expected<U, E>
     */
    template <class Op, trait::concept_t<expected_details::is_same_error_expected_v<Expected, std::invoke_result_t<Op, T>>> = nullptr>
    constexpr auto and_then(Op && op) const&->std::invoke_result_t<Op, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), value_noexcept());
    }
    template <class Op, trait::concept_t<expected_details::is_same_error_expected_v<Expected, std::invoke_result_t<Op, T>>> = nullptr>
    constexpr auto and_then(Op && op)&->std::invoke_result_t<Op, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), value_noexcept());
    }
    template <class Op, trait::concept_t<expected_details::is_same_error_expected_v<Expected, std::invoke_result_t<Op, T>>> = nullptr>
    constexpr auto and_then(Op && op)&&->std::invoke_result_t<Op, T>
    {
        if (!valid()) return {unexpect_tag_v, std::move(error_noexcept())};
        return trait::invoke_constexpr(std::forward<Op>(op), std::move(value_noexcept()));
    }

    /**
     * @brief or_else
     * @details Calls op if the expected is invalid, otherwise returns the 'value()' value of self.
     * @param op E -> Expected<T, F>
     * @return Expected<U, E>
     */
    template <class Op, trait::concept_t<expected_details::is_same_value_expected_v<Expected, std::invoke_result_t<Op, E>>> = nullptr>
    constexpr auto or_else(Op && op) const&->std::invoke_result_t<Op, E>
    {
        if (valid()) return {expect_tag_v, value_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), error_noexcept());
    }
    template <class Op, trait::concept_t<expected_details::is_same_value_expected_v<Expected, std::invoke_result_t<Op, E>>> = nullptr>
    constexpr auto or_else(Op && op)&->std::invoke_result_t<Op, E>
    {
        if (valid()) return {expect_tag_v, value_noexcept()};
        return trait::invoke_constexpr(std::forward<Op>(op), error_noexcept());
    }
    template <class Op, trait::concept_t<expected_details::is_same_value_expected_v<Expected, std::invoke_result_t<Op, E>>> = nullptr>
    constexpr auto or_else(Op && op)&&->std::invoke_result_t<Op, E>
    {
        if (valid()) return {expect_tag_v, std::move(value_noexcept())};
        return trait::invoke_constexpr(std::forward<Op>(op), std::move(error_noexcept()));
    }

    /**
     * @brief mach
     * int x = expected.mach<int>(ok_func, err_func);
     * @tparam R return type
     * @param fn_ok ok
     * @param fn_err err
     * @return std::common_type_t<decltype(fn_ok(value())), decltype(fn_err(error()))>
     */
    template <class R, class FN_OK, class FN_ERR>
    constexpr R mach(FN_OK && fn_ok, FN_ERR && fn_err) const&
    {
        if (valid()) {
            if constexpr (std::is_invocable_r_v<R, FN_OK, T>) {
                return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok), value_noexcept());
            } else {
                return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok));
            }
        }
        if constexpr (std::is_invocable_r_v<R, FN_ERR, E>) {
            return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err), error_noexcept());
        } else {
            return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err));
        }
    }
    template <class R, class FN_OK, class FN_ERR>
    constexpr R mach(FN_OK && fn_ok, FN_ERR && fn_err)&
    {
        if (valid()) {
            if constexpr (std::is_invocable_r_v<R, FN_OK, T>) {
                return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok), value_noexcept());
            } else {
                return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok));
            }
        }
        if constexpr (std::is_invocable_r_v<R, FN_ERR, E>) {
            return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err), error_noexcept());
        } else {
            return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err));
        }
    }
    template <class R, class FN_OK, class FN_ERR>
    constexpr R mach(FN_OK && fn_ok, FN_ERR && fn_err)&&
    {
        if (valid()) {
            if constexpr (std::is_invocable_r_v<R, FN_OK, T>) {
                return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok), std::move(value_noexcept()));
            } else {
                return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok));
            }
        }
        if constexpr (std::is_invocable_r_v<R, FN_ERR, E>) {
            return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err), std::move(error_noexcept()));
        } else {
            return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err));
        }
    }
};

template <class T, class E, class... Args>
inline constexpr Expected<T, E> aggregateExpected(const Expected<Args, E>&... args)
{
    if ((... && (args.valid()))) {
        return T{*args...};
    }
    std::optional<E> e;
    auto f = [&](const E& ee) {if(!e){e = ee;} return Unexpect(ee); };
    (..., ((void)args.emap(f)));
    return Unexpect(*e);
}

} // namespace rib
