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

template <class E>
class Unexpected;

struct Unexpect_tag
{
    explicit Unexpect_tag() = default;
};
inline constexpr Unexpect_tag unexpect_tag_v{};

/**
 * @brief Expected value
 * like `Result` in `rust`
 * @tparam T Ok (require destructible)
 * @tparam E Err (require destructible)
 */
template <class T, class E, trait::concept_t<std::is_destructible_v<T> && std::is_destructible_v<E>> = nullptr>
class [[nodiscard]] Expected {
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

    template <class>
    struct is_same_error_expected : std::false_type
    {
    };
    template <class U>
    struct is_same_error_expected<Expected<U, E>> : std::true_type
    {
    };

    template <class>
    struct unwrap_helper
    {
        using type = Expected;
    };
    template <class U, class F>
    struct unwrap_helper<Expected<U, F>>
    {
        using type = Expected<U, F>;
    };

public:
    using OkType = T;
    using ErrType = E;
    using Unwrap_t = typename unwrap_helper<T>::type;

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

    /// construct by err value
    template <class F, trait::concept_t<std::is_constructible_v<E, F>> = nullptr>
    constexpr Expected(const Unexpected<F>& err) noexcept(std::is_nothrow_constructible_v<E, F>)
        : payload(std::in_place_type<Err>, err.value()) {}
    /// construct by err value
    template <class F, trait::concept_t<std::is_constructible_v<E, F>> = nullptr>
    constexpr Expected(Unexpected<F> && err) noexcept(std::is_nothrow_constructible_v<E, F&&>)
        : payload(std::in_place_type<Err>, std::move(err.value())) {}
    /// construct by err value
    template <class... Args, trait::concept_t<std::is_constructible_v<E, Args&&...>> = nullptr>
    constexpr Expected(Unexpect_tag, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
        : payload(std::in_place_type<Err>, std::forward<Args>(args)...) {}

    /// copy assign
    constexpr Expected& operator=(const Expected&) = default;
    /// move assign
    constexpr Expected& operator=(Expected&&) = default;

    /// has ok value
    constexpr bool valid() const noexcept { return std::holds_alternative<Ok>(payload); }
    /// has ok value
    constexpr operator bool() const noexcept { return valid(); }

    /// value (exception)
    constexpr const T& value() const& noexcept(false)
    {
        if (!valid()) throw BadExpectedAccess{};
        return std::get<Ok>(payload).v;
    }
    /// value (exception)
    constexpr T& value() & noexcept(false)
    {
        if (!valid()) throw BadExpectedAccess{};
        return std::get<Ok>(payload).v;
    }
    /// value (exception)
    constexpr T value() && noexcept(false)
    {
        if (!valid()) throw BadExpectedAccess{};
        return std::move(std::get<Ok>(payload).v);
    }

    /// value (no exception)
    constexpr const T& operator*() const& noexcept { return std::get<Ok>(payload).v; }
    /// value (no exception)
    constexpr T& operator*() & noexcept { return std::get<Ok>(payload).v; }
    /// value (no exception)
    constexpr T operator*() && noexcept { return std::move(std::get<Ok>(payload).v); }

    /// if `*this` has value returns contained value, otherwise returns default_value
    template <class U>
    constexpr T value_or(U && default_value) const&
    {
        if (valid()) return **this;
        return static_cast<T>(std::forward<U>(default_value));
    }
    /// if *this has value returns contained value, otherwise returns default_value
    template <class U>
    constexpr T value_or(U && default_value)&&
    {
        if (valid()) return std::move(**this);
        return static_cast<T>(std::forward<U>(default_value));
    }

    /// if `* this` has value returns contained value, otherwise returns default construction
    template <class U = T, trait::concept_t<std::is_same_v<T, U> && (std::is_default_constructible_v<T> || std::is_aggregate_v<T>)> = nullptr>
    constexpr T value_or_default() const&
    {
        if (valid()) return **this;
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
        if (valid()) return std::move(**this);
        if constexpr (std::is_aggregate_v<T>) {
            return T{};
        } else {
            return T();
        }
    }

    /// pointer (no exception)
    constexpr const T* operator->() noexcept { return &**this; }
    /// pointer (no exception)
    constexpr T* operator->() const noexcept { return &**this; }

    /// error value (exception)
    constexpr const E& error() const& noexcept(false)
    {
        if (valid()) throw BadExpectedAccess{};
        return std::get<Err>(payload).v;
    }
    /// error value (exception)
    constexpr E& error() & noexcept(false)
    {
        if (valid()) throw BadExpectedAccess{};
        return std::get<Err>(payload).v;
    }
    /// error value (exception)
    constexpr E error() && noexcept(false)
    {
        if (valid()) throw BadExpectedAccess{};
        return std::move(std::get<Err>(payload).v);
    }

    /// error value (no exception)
    constexpr const E& error_noexcept() const& noexcept
    {
        return std::get<Err>(payload).v;
    }
    /// error value (no exception)
    constexpr E& error_noexcept() & noexcept
    {
        return std::get<Err>(payload).v;
    }
    /// error value (no exception)
    constexpr E error_noexcept() && noexcept
    {
        return std::move(std::get<Err>(payload).v);
    }

    /// regenerate Unexpected
    constexpr Unexpected<E> unexpected() const& noexcept(false)
    {
        return Unexpected(error());
    }
    /// regenerate Unexpected
    constexpr Unexpected<E> unexpected() && noexcept(false)
    {
        return Unexpected(std::move(error()));
    }

    /// optional<T>
    constexpr std::optional<T> optional() const& noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        if (valid()) return **this;
        return std::nullopt;
    }
    /// optional<T>
    constexpr std::optional<T> optional() && noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        if (valid()) return std::move(**this);
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
    friend constexpr bool operator==(const Expected& lhs, const Unexpected<X>& rhs) noexcept
    {
        if (!lhs) return lhs.error() == rhs.value();
        return false;
    }
    /// unequally compare
    template <class X>
    friend constexpr bool operator!=(const Expected& lhs, const Unexpected<X>& rhs) noexcept
    {
        return !(lhs == rhs);
    }
    /// equally compare
    template <class X>
    friend constexpr bool operator==(const Unexpected<X>& lhs, const Expected& rhs) noexcept
    {
        if (!rhs) return lhs.value() == rhs.error();
        return false;
    }
    /// unequally compare
    template <class X>
    friend constexpr bool operator!=(const Unexpected<X>& lhs, const Expected& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    /// swap value
    void swap(Expected & rhs) noexcept(noexcept(std::swap(payload.swap(rhs.payload))))
    {
        payload.swap(rhs.payload);
    }

    /**
     * @brief map
     * @param f invoke(f, value())
     * @return Expected<decltype(f(value())), E>
     */
    template <class F>
    constexpr auto map(F && f) const&->Expected<std::invoke_result_t<F, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<F>(f), **this);
    }
    template <class F>
    constexpr auto map(F && f)&->Expected<std::invoke_result_t<F, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<F>(f), **this);
    }
    template <class F>
    constexpr auto map(F && f)&&->Expected<std::invoke_result_t<F, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<F>(f), std::move(**this));
    }

    /**
     * @brief bind
     * @param f invoke(f, value()) -> Expected<U, E>
     * @return decltype(f(value()))
     */
    template <class F, trait::concept_t<is_same_error_expected<std::invoke_result_t<F, T>>::value> = nullptr>
    constexpr auto bind(F && f) const&->std::invoke_result_t<F, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<F>(f), **this);
    }
    template <class F, trait::concept_t<is_same_error_expected<std::invoke_result_t<F, T>>::value> = nullptr>
    constexpr auto bind(F && f)&->std::invoke_result_t<F, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<F>(f), **this);
    }
    template <class F, trait::concept_t<is_same_error_expected<std::invoke_result_t<F, T>>::value> = nullptr>
    constexpr auto bind(F && f)&&->std::invoke_result_t<F, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        return trait::invoke_constexpr(std::forward<F>(f), std::move(**this));
    }

    /**
     * @brief then
     * @param f invoke(f, *this) -> {Expected<U, X> or U}
     * @return if f returns Expected then f(*this) otherwise Expected<f(*this), E>
     */
    template <class F>
    constexpr auto then(F && f) const&->typename Expected<std::invoke_result_t<F, Expected>, E>::Unwrap_t
    {
        return trait::invoke_constexpr(std::forward<F>(f), *this);
    }
    template <class F>
    constexpr auto then(F && f)&->typename Expected<std::invoke_result_t<F, Expected>, E>::Unwrap_t
    {
        return trait::invoke_constexpr(std::forward<F>(f), *this);
    }
    template <class F>
    constexpr auto then(F && f)&&->typename Expected<std::invoke_result_t<F, Expected>, E>::Unwrap_t
    {
        return trait::invoke_constexpr(std::forward<F>(f), std::move(*this));
    }

    /**
     * @brief catch_error
     * @param f invoke(f, error()) -> T
     * @return Expected<T, E>
     */
    template <class F>
    constexpr Expected catch_error(F && f) const&
    {
        if (valid()) return *this;
        return trait::invoke_constexpr(std::forward<F>(f), error_noexcept());
    }
    template <class F>
    constexpr Expected catch_error(F && f)&
    {
        if (valid()) return *this;
        return trait::invoke_constexpr(std::forward<F>(f), error_noexcept());
    }
    template <class F>
    constexpr Expected catch_error(F && f)&&
    {
        if (valid()) return std::move(*this);
        return trait::invoke_constexpr(std::forward<F>(f), std::move(error_noexcept()));
    }

    /**
     * @brief emap
     * @param f invoke(f, error()) -> X
     * @return Expected<T, decltype(f(error()))>>
     */
    template <class F>
    constexpr auto emap(F && f) const&->Expected<T, std::invoke_result_t<F, E>>
    {
        if (valid()) return **this;
        return {unexpect_tag_v, trait::invoke_constexpr(std::forward<F>(f), error())};
    }
    template <class F>
    constexpr auto emap(F && f)&->Expected<T, std::invoke_result_t<F, E>>
    {
        if (valid()) return **this;
        return {unexpect_tag_v, trait::invoke_constexpr(std::forward<F>(f), error())};
    }
    template <class F>
    constexpr auto emap(F && f)&&->Expected<T, std::invoke_result_t<F, E>>
    {
        if (valid()) return std::move(**this);
        return {unexpect_tag_v, trait::invoke_constexpr(std::forward<F>(f), std::move(error()))};
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
        if (valid()) return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok), **this);
        return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err), error_noexcept());
    }
    template <class R, class FN_OK, class FN_ERR>
    constexpr R mach(FN_OK && fn_ok, FN_ERR && fn_err)&
    {
        if (valid()) return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok), **this);
        return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err), error_noexcept());
    }
    template <class R, class FN_OK, class FN_ERR>
    constexpr R mach(FN_OK && fn_ok, FN_ERR && fn_err)&&
    {
        if (valid()) return trait::invoke_constexpr(std::forward<FN_OK>(fn_ok), std::move(**this));
        return trait::invoke_constexpr(std::forward<FN_ERR>(fn_err), std::move(error_noexcept()));
    }
};

/**
 * @brief indicate unexpect value
 * @tparam E
 */
template <class E>
class Unexpected
{
    E payload;

public:
    constexpr Unexpected(const E& e) : payload(e) {}
    constexpr Unexpected(E&& e) : payload(std::move(e)) {}

    template <class... Args>
    constexpr Unexpected(Args&&... args) : payload(std::forward<Args>(args)...) {}

    constexpr const E& value() const& { return payload; }
    constexpr E& value() & { return payload; }
    constexpr E value() && { return std::move(payload); }
};
Unexpected(const char[])->Unexpected<const char*>;

} // namespace rib
