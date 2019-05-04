#pragma once

#include <type_traits>

/// type utility
namespace rib::trait
{

/**
 * @brief check whether Instance is specialization ob Template
 * @tparam Template 
 * @tparam Instance 
 * @sa is_template_specialized_by_value
 * @sa is_template_specialized_by_type_v
 */
template <template <class...> class Template, class Instance>
struct is_template_specialized_by_type : std::false_type
{
};
/**
 * @brief check whether Instance is specialization ob Template
 * @tparam Template 
 * @tparam Instance 
 * @sa is_template_specialized_by_value
 * @sa is_template_specialized_by_type_v
 */
template <template <class...> class Template, class... Args>
struct is_template_specialized_by_type<Template, Template<Args...>> : std::true_type
{
};
/**
 * @brief helper variable template: is_template_specialized_by_type
 * @tparam Template 
 * @tparam Instance 
 */
template <template <class...> class Template, class Instance>
constexpr bool is_template_specialized_by_type_v = is_template_specialized_by_type<Template, Instance>::value;

/**
 * @brief check whether Instance is specialization ob Template
 * @tparam Template 
 * @tparam Instance 
 * @sa is_template_specialized_by_type
 * @sa is_template_specialized_by_value_v
 */
template <template <auto...> class Template, class Instance>
struct is_template_specialized_by_value : std::false_type
{
};
/**
 * @brief check whether Instance is specialization ob Template
 * @tparam Template 
 * @tparam Instance 
 * @sa is_template_specialized_by_type
 * @sa is_template_specialized_by_value_v
 */
template <template <auto...> class Template, auto... Args>
struct is_template_specialized_by_value<Template, Template<Args...>> : std::true_type
{
};
/**
 * @brief helper variable template: is_template_specialized_by_value
 * @tparam Template 
 * @tparam Instance 
 */
template <template <auto...> class Template, class Instance>
constexpr bool is_template_specialized_by_value_v = is_template_specialized_by_value<Template, Instance>::value;

/**
 * @brief copy const qualification
 * @tparam From
 * @tparam To
 */
template <class From, class To>
struct copy_const : std::conditional<std::is_const_v<From>,
                                     const std::remove_const_t<To>,
                                     std::remove_const_t<To>>
{
};
/**
 * @brief helper alias template: copy_const
 * @tparam T
 * @tparam U
 */
template <class T, class U>
using copy_const_t = typename copy_const<T, U>::type;

static_assert(std::is_same_v<copy_const_t<int, long>, long>);
static_assert(std::is_same_v<copy_const_t<const int, long>, const long>);
static_assert(std::is_same_v<copy_const_t<int, const long>, long>);
static_assert(std::is_same_v<copy_const_t<const int, const long>, const long>);

/**
 * @brief concept_t
 * for SFINAE
 */
template <bool f>
using concept_t = std::enable_if_t<f, std::nullptr_t>;

} // namespace rib::trait
