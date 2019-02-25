#pragma once
#include <utility>

/// handling type information
namespace rib::type::handle
{

/**
 * @brief type adapter
 * @details type information at copyable
 * @tparam T type
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
struct TemplateAdaperByType
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
struct TemplateAdaperByValue
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

} // namespace rib::type::handle
