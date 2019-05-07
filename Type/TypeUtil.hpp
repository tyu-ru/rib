#pragma once
#include <type_traits>

/// handling type information
namespace rib::type
{

/**
 * @brief type adapter
 * @details type information at copyable
 * @tparam T type
 * @sa std::type_identity (c++20)
 */
template <class T>
struct TypeAdapter
{
    /// type
    using type = T;
};

/**
 * @brief template adapter(by type)
 * @details handling template(by type)
 * @tparam Template class template
 */
template <template <class...> class Template>
struct TemplateAdapterByType
{
    /**
     * @brief template specialization
     * @details equal Template<Args...>
     * @tparam Args
     */
    template <class... Args>
    using Specialize = Template<Args...>;
    /**
     * @brief TypeAdapter
     * @details equal TypeAdapter<Template<Args...>>
     * @tparam Args
     */
    template <class... Args>
    using TypeAdapter = TypeAdapter<Template<Args...>>;
};
/**
 * @brief template adapter(by value)
 * @details handling template(by value)
 * @tparam Template class template
 */
template <template <auto...> class Template>
struct TemplateAdapterByValue
{
    /**
     * @brief template specialization
     * @details equal Template<args...>
     * @tparam args
     */
    template <auto... args>
    using Specialize = Template<args...>;
    /**
     * @brief TypeAdapter
     * @details equal TypeAdapter<Template<args...>>
     * @tparam args
     */
    template <auto... args>
    using TypeAdapter = TypeAdapter<Template<args...>>;
};

template <class>
struct TemplateAdapterFromType;
template <template <class...> class Template, class... Args>
struct TemplateAdapterFromType<Template<Args...>>
{
    using type = TemplateAdapterByType<Template>;
};

/**
 * @brief template argument binder
 * @tparam Template class template
 * @tparam BoundArgs binding types
 */
template <template <class...> class Template, class... BoundArgs>
struct TemplateBind
{
    template <class... Args>
    struct type : Template<BoundArgs..., Args...>
    {
    };
    template <class Arg1>
    struct type1 : type<Arg1>
    {
    };
    template <class Arg1, class Arg2>
    struct type2 : type<Arg1, Arg2>
    {
    };
    template <class Arg1, class Arg2, class Arg3>
    struct type3 : type<Arg1, Arg2, Arg3>
    {
    };
    template <class Arg1, class Arg2, class Arg3, class Arg4>
    struct type4 : type<Arg1, Arg2, Arg3, Arg4>
    {
    };
};

static_assert(TemplateBind<std::is_same, int>::type1<int>::value);
static_assert(TemplateAdapterByType<TemplateBind<std::is_same, int>::type1>::Specialize<int>::value);

} // namespace rib::type
