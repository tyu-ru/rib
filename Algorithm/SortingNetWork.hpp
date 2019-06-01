
#include <tuple>
#include <type_traits>
#include <iterator>

namespace srtnw
{

template <std::size_t i_, std::size_t j_>
struct SwapPair
{
    static constexpr std::size_t i = i_, j = j_;
};

template <class RandomAccessIterator>
inline constexpr void compair_swap(RandomAccessIterator v, std::size_t i, std::size_t j)
{
    if constexpr (std::is_arithmetic_v<typename std::iterator_traits<decltype(v)>::value_type>) {
        auto v_i = std::move(v[i]);
        auto v_j = std::move(v[j]);
        v[i] = v_i < v_j ? v_i : v_j;
        v[j] = v_i < v_j ? v_j : v_i;
    } else {
        using std::swap;
        if (v[j] < v[i]) swap(v[i], v[j]);
    }
}

template <class SNW>
class SortingNetwork
{
    template <class IT, class... Args>
    inline static constexpr void sort_impl(IT first, std::tuple<Args...>*)
    {
        (..., compair_swap(first, Args::i, Args::j));
    }

public:
    template <class RandomAccessIterator>
    inline static constexpr void sort(RandomAccessIterator first) { sort_impl(first, static_cast<typename SNW::type*>(nullptr)); }
};

template <std::size_t n>
struct BoseNelson
{
private:
    template <class... Args>
    using unwrap_tuple_cat_t = decltype(std::tuple_cat(std::declval<typename Args::type>()...));

    template <std::size_t i, /* value of first element in sequence 1 */
              std::size_t x, /* length of sequence 1 */
              std::size_t j, /* value of first element in sequence 2 */
              std::size_t y> /* length of sequence 2 */
    struct Pbracket
    {
        static constexpr std::size_t a = x / 2;
        static constexpr std::size_t b = (x & 1) ? (y / 2) : ((y + 1) / 2);

        using type = unwrap_tuple_cat_t<Pbracket<i, a, j, b>,
                                        Pbracket<i + a, x - a, j + b, y - b>,
                                        Pbracket<i + a, x - a, j, b>>;
    };
    template <std::size_t i, std::size_t j>
    struct Pbracket<i, 1, j, 1>
    {
        using type = std::tuple<SwapPair<i, j>>;
    };
    template <std::size_t i, std::size_t j>
    struct Pbracket<i, 1, j, 2>
    {
        using type = std::tuple<SwapPair<i, j + 1>, SwapPair<i, j>>;
    };
    template <std::size_t i, std::size_t j>
    struct Pbracket<i, 2, j, 1>
    {
        using type = std::tuple<SwapPair<i, j>, SwapPair<i + 1, j>>;
    };

    template <std::size_t i, std::size_t m, class /* for detection idiom */ = void>
    struct Pstar
    {
        static constexpr std::size_t a = m / 2;
        using type = unwrap_tuple_cat_t<Pstar<i, a>,
                                        Pstar<i + a, m - a>,
                                        Pbracket<i, a, i + a, m - a>>;
    };
    template <std::size_t i, std::size_t m>
    struct Pstar<i, m, std::enable_if_t<(m < 2)>>
    {
        using type = std::tuple<>;
    };

public:
    using type = typename Pstar<0, n>::type;
};
template <std::size_t n>
using BoseNelson_t = typename BoseNelson<n>::type;

static_assert(std::is_same_v<typename BoseNelson<0>::type,
                             std::tuple<>>);
static_assert(std::is_same_v<typename BoseNelson<1>::type,
                             std::tuple<>>);
static_assert(std::is_same_v<typename BoseNelson<2>::type,
                             std::tuple<SwapPair<0, 1>>>);
static_assert(std::is_same_v<typename BoseNelson<8>::type,
                             std::tuple<SwapPair<0, 1>,
                                        SwapPair<2, 3>,
                                        SwapPair<0, 2>,
                                        SwapPair<1, 3>,
                                        SwapPair<1, 2>,
                                        SwapPair<4, 5>,
                                        SwapPair<6, 7>,
                                        SwapPair<4, 6>,
                                        SwapPair<5, 7>,
                                        SwapPair<5, 6>,
                                        SwapPair<0, 4>,
                                        SwapPair<1, 5>,
                                        SwapPair<1, 4>,
                                        SwapPair<2, 6>,
                                        SwapPair<3, 7>,
                                        SwapPair<3, 6>,
                                        SwapPair<2, 4>,
                                        SwapPair<3, 5>,
                                        SwapPair<3, 4>>>);
} // namespace srtnw
