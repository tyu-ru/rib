#pragma once

#include <variant>
#include <optional>
#include <functional>

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

template <class T, class E, class /* for detection ideom */ = void>
class Expected;

template <class T, class E>
class[[nodiscard]] Expected<T, E,
                            std::enable_if_t<std::is_destructible_v<T> && std::is_destructible_v<E>>>
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
    using Unwrap_t = typename unwrap_helper<T>::type;

public:
    /// [deleted] default constructor
    constexpr Expected() = delete;
    /// copy constructor
    constexpr Expected(const Expected& other) noexcept(std::is_nothrow_copy_constructible_v<decltype(payload)>) = default;
    /// move constructor
    constexpr Expected(Expected && other) noexcept(std::is_nothrow_move_constructible_v<decltype(payload)>) = default;

    /// construct by ok value
    template <class... Args, std::enable_if_t<std::is_constructible_v<T, Args&&...>, std::nullptr_t> = nullptr>
    constexpr Expected(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
        : payload(std::in_place_type<Ok>, std::forward<Args>(args)...) {}

    /// construct by err value
    template <class F, std::enable_if_t<std::is_constructible_v<E, F>, std::nullptr_t> = nullptr>
    constexpr Expected(const Unexpected<F>& err) noexcept(std::is_nothrow_constructible_v<E, F>)
        : payload(std::in_place_type<Err>, err.value()) {}
    /// construct by err value
    template <class F, std::enable_if_t<std::is_constructible_v<E, F>, std::nullptr_t> = nullptr>
    constexpr Expected(Unexpected<F> && err) noexcept(std::is_nothrow_constructible_v<E, F&&>)
        : payload(std::in_place_type<Err>, std::move(err.value())) {}
    /// construct by err value
    template <class... Args, std::enable_if_t<std::is_constructible_v<E, Args&&...>, std::nullptr_t> = nullptr>
    constexpr Expected(Unexpect_tag, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
        : payload(std::in_place_type<Err>, std::forward<Args>(args)...) {}

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
    /// if `*this` has value returns contained value, otherwise returns default_value
    template <class U>
    constexpr T value_or(U && default_value)&&
    {
        if (valid()) return std::move(**this);
        return static_cast<T>(std::forward<U>(default_value));
    }

    /// if `* this` has value returns contained value, otherwise returns default construction
    template <class U = T, std::enable_if_t<std::is_same_v<T, U> && (std::is_default_constructible_v<T> || std::is_aggregate_v<T>), std::nullptr_t> = nullptr>
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
    template <class U = T, std::enable_if_t<std::is_same_v<T, U> && (std::is_default_constructible_v<T> || std::is_aggregate_v<T>), std::nullptr_t> = nullptr>
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
    constexpr Unexpected<E> unexpected() const&
    {
        return Unexpected(error());
    }
    /// regenerate Unexpected
    constexpr Unexpected<E> unexpected()&&
    {
        return Unexpected(std::move(error()));
    }

    /// optional<T>
    constexpr std::optional<T> optional() const&
    {
        if (valid()) return **this;
        return std::nullopt;
    }
    /// optional<T>
    constexpr std::optional<T> optional()&&
    {
        if (valid()) return std::move(**this);
        return std::nullopt;
    }

    /// equally compare
    friend constexpr bool operator==(const Expected& lhs, const Expected& rhs)
    {
        if (lhs && rhs) return *lhs == *rhs;
        if (!lhs && !rhs) return lhs.error() == rhs.error();
        return false;
    }
    /// unequally compare
    friend constexpr bool operator!=(const Expected& lhs, const Expected& rhs)
    {
        return !(lhs == rhs);
    }

    /// equally compare
    template <class U>
    friend constexpr bool operator==(const Expected& lhs, const U& rhs)
    {
        if (lhs) return *lhs == rhs;
        return false;
    }
    /// unequally compare
    template <class U>
    friend constexpr bool operator!=(const Expected& lhs, const U& rhs)
    {
        return !(lhs == rhs);
    }
    /// equally compare
    template <class U>
    friend constexpr bool operator==(const U& lhs, const Expected& rhs)
    {
        if (rhs) return lhs == *rhs;
        return false;
    }
    /// unequally compare
    template <class U>
    friend constexpr bool operator!=(const U& lhs, const Expected& rhs)
    {
        return !(lhs == rhs);
    }

    /// equally compare
    friend constexpr bool operator==(const Expected& lhs, const Unexpected<E>& rhs)
    {
        if (!lhs) return lhs.error() == rhs.value();
        return false;
    }
    /// unequally compare
    friend constexpr bool operator!=(const Expected& lhs, const Unexpected<E>& rhs)
    {
        return !(lhs == rhs);
    }
    /// equally compare
    friend constexpr bool operator==(const Unexpected<E>& lhs, const Expected& rhs)
    {
        if (!rhs) return lhs.value() == rhs.error();
        return false;
    }
    /// unequally compare
    friend constexpr bool operator!=(const Unexpected<E>& lhs, const Expected& rhs)
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
    constexpr auto map(F f) const&->Expected<std::invoke_result_t<F, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, **this);
        } else {
            return f(**this);
        }
    }
    template <class F>
    constexpr auto map(F f)&->Expected<std::invoke_result_t<F, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, **this);
        } else {
            return f(**this);
        }
    }
    template <class F>
    constexpr auto map(F f)&&->Expected<std::invoke_result_t<F, T>, E>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, std::move(**this));
        } else {
            return f(std::move(**this));
        }
    }

    /**
     * @brief bind
     * @param f invoke(f, value()) -> Expected<U, E>
     * @return decltype(f(value()))
     */
    template <class F, std::enable_if_t<is_same_error_expected<std::invoke_result_t<F, T>>::value, std::nullptr_t> = nullptr>
    constexpr auto bind(F f) const&->std::invoke_result_t<F, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, **this);
        } else {
            return f(**this);
        }
    }
    template <class F, std::enable_if_t<is_same_error_expected<std::invoke_result_t<F, T>>::value, std::nullptr_t> = nullptr>
    constexpr auto bind(F f)&->std::invoke_result_t<F, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, **this);
        } else {
            return f(**this);
        }
    }
    template <class F, std::enable_if_t<is_same_error_expected<std::invoke_result_t<F, T>>::value, std::nullptr_t> = nullptr>
    constexpr auto bind(F f)&&->std::invoke_result_t<F, T>
    {
        if (!valid()) return {unexpect_tag_v, error_noexcept()};
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, std::move(**this));
        } else {
            return f(std::move(**this));
        }
    }

    template <class F>
    constexpr auto then(F f) const&->typename Expected<std::invoke_result_t<F, Expected>, E>::Unwrap_t
    {
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, *this);
        } else {
            return f(*this);
        }
    }
    template <class F>
    constexpr auto then(F f)&->typename Expected<std::invoke_result_t<F, Expected>, E>::Unwrap_t
    {
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, *this);
        } else {
            return f(*this);
        }
    }
    template <class F>
    constexpr auto then(F f)&&->typename Expected<std::invoke_result_t<F, Expected>, E>::Unwrap_t
    {
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, std::move(*this));
        } else {
            return f(std::move(*this));
        }
    }

    template <class F>
    constexpr Expected catch_error(F f) const&
    {
        if (valid()) return *this;
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, error());
        } else {
            return f(error());
        }
    }
    template <class F>
    constexpr Expected catch_error(F f)&
    {
        if (valid()) return *this;
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, error());
        } else {
            return f(error());
        }
    }
    template <class F>
    constexpr Expected catch_error(F f)&&
    {
        if (valid()) return std::move(*this);
        if constexpr (std::is_member_pointer_v<F>) {
            return std::invoke(f, std::move(error()));
        } else {
            return f(std::move(error()));
        }
    }

    template <class F>
    constexpr auto emap(F f) const&->Expected<T, std::invoke_result_t<F, E>>
    {
        if (valid()) return **this;
        if constexpr (std::is_member_pointer_v<F>) {
            return {unexpect_tag_v, std::invoke(f, error())};
        } else {
            return {unexpect_tag_v, f(error())};
        }
    }
    template <class F>
    constexpr auto emap(F f)&->Expected<T, std::invoke_result_t<F, E>>
    {
        if (valid()) return **this;
        if constexpr (std::is_member_pointer_v<F>) {
            return {unexpect_tag_v, std::invoke(f, error())};
        } else {
            return {unexpect_tag_v, f(error())};
        }
    }
    template <class F>
    constexpr auto emap(F f)&&->Expected<T, std::invoke_result_t<F, E>>
    {
        if (valid()) return std::move(**this);
        if constexpr (std::is_member_pointer_v<F>) {
            return {unexpect_tag_v, std::invoke(f, std::move(error()))};
        } else {
            return {unexpect_tag_v, f(std::move(error()))};
        }
    }
};

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

static_assert(!std::is_default_constructible_v<Expected<int, int>>);
static_assert(std::is_copy_constructible_v<Expected<int, int>>);
static_assert(std::is_move_constructible_v<Expected<int, int>>);

static_assert(std::is_constructible_v<Expected<int, int>, int>);
static_assert(std::is_constructible_v<Expected<int, int>, int>);
static_assert(std::is_constructible_v<Expected<int, int>, Unexpected<int>>);
static_assert(std::is_constructible_v<Expected<int, int>, Unexpected<int>&&>);
static_assert(std::is_constructible_v<Expected<int, int>, Unexpect_tag, int>);

static_assert(std::is_same_v<Expected<int, int>::Unwrap_t, Expected<int, int>>);
static_assert(std::is_same_v<Expected<Expected<int, int>, int>::Unwrap_t, Expected<int, int>>);

static_assert([] {
    Expected<int, long> e = 1;
    return e.valid() && e;
}());
static_assert([] {
    Expected<int, long> e = Unexpected<long>(1);
    return !e.valid() && !e;
}());

static_assert([] {
    Expected<int, long> e = 1;
    return e.value() == 1 && *e == 1;
}());
static_assert([] {
    Expected<int, long> e1 = 1;
    Expected<int, long> e2 = Unexpected<long>(1);
    return e1.value_or(2) == 1 && e2.value_or(2) == 2;
}());
static_assert([] {
    Expected<int, long> e1 = 1;
    Expected<int, long> e2 = Unexpected<long>(1);
    return e1.value_or_default() == 1 && e2.value_or_default() == 0;
}());
static_assert([] {
    struct A
    {
        int a;
    };
    Expected<A, long> e = A{1};
    return e->a == 1;
}());

static_assert([] {
    return Expected<int, long>(Unexpected(1)).error() == 1 &&
           Expected<int, long>(unexpect_tag_v, 1).error() == 1;
}());

static_assert([] {
    return Expected<int, int>(Expected<int, int>{Unexpected(1)}.unexpected()).error() == 1;
}());

static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().optional()), std::optional<int>>);

static_assert([] {
    return Expected<int, int>(1).optional() == 1 &&
           Expected<int, int>(Unexpected(1)).optional() == std::nullopt;
}());

static_assert([] {
    return Expected<int, int>(1) == Expected<int, int>(1) &&
           Expected<int, int>(1) != Expected<int, int>(Unexpected(1)) &&
           Expected<int, int>(Unexpected(1)) == Expected<int, int>(Unexpected(1));
}());
static_assert([] {
    return Expected<int, int>(1) == 1 &&
           Expected<int, int>(Unexpected(1)) != 1 &&
           Expected<int, int>(1) != Unexpected(1) &&
           Expected<int, int>(Unexpected(1)) == Unexpected(1);
}());
static_assert([] {
    return 1 == Expected<int, int>(1) &&
           1 != Expected<int, int>(Unexpected(1)) &&
           Unexpected(1) != Expected<int, int>(1) &&
           Unexpected(1) == Expected<int, int>(Unexpected(1));
}());

static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().map(std::declval<double(int)>())),
                             Expected<double, long>>);
static_assert([] {
    auto lmd = [](int x) { return x + 1; };
    return Expected<int, int>(1).map(lmd) == 2 &&
           Expected<int, int>(Unexpected(1)).map(lmd) == Unexpected(1);
}());

static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().bind(std::declval<Expected<double, long>(int)>())),
                             Expected<double, long>>);
static_assert([] {
    auto lmd = [](int x) -> Expected<int, int> { if (x == 1) return x + 1; return Unexpected(2); };
    return Expected<int, int>(1).bind(lmd) == 2 &&
           Expected<int, int>(2).bind(lmd) == Unexpected(2) &&
           Expected<int, int>(Unexpected(1)).bind(lmd) == Unexpected(1);
}());

static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().then(std::declval<Expected<double, float>(Expected<int, long>)>())),
                             Expected<double, float>>);
static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().then(std::declval<double(Expected<int, long>)>())),
                             Expected<double, long>>);
static_assert([] {
    auto lmd1 = [](Expected<int, int>) { return 'a'; };
    auto lmd2 = [](Expected<int, int>) { return Expected<long, int>(2); };
    return Expected<int, int>(1).then(lmd1) == 'a' &&
           Expected<int, int>(Unexpected(1)).then(lmd1) == 'a' &&
           Expected<int, int>(1).then(lmd2) == 2 &&
           Expected<int, int>(Unexpected(1)).then(lmd2) == 2;
}());

static_assert(std::is_same_v<decltype(std::declval<Expected<int, int>>().catch_error(std::declval<int(int)>())),
                             Expected<int, int>>);
static_assert([] {
    auto lmd = [](int x) { return x + 1; };
    return Expected<int, int>(1).catch_error(lmd) == 1 &&
           Expected<int, int>(Unexpected(2)).catch_error(lmd) == 3;
}());

static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().emap(std::declval<int(long)>())),
                             Expected<int, int>>);
static_assert([] {
    auto lmd = [](int x) { return x + 1; };
    return Expected<int, int>(1).emap(lmd) == 1 &&
           Expected<int, int>(Unexpected(2)).emap(lmd).error() == 3;
}());

} // namespace rib
