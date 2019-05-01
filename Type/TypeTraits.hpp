#pragma once

#include <type_traits>

/// type utility
namespace rib::type
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
 * @tparam T 
 * @tparam U 
 */
template <class T, class U>
struct copy_const : std::conditional<std::is_const_v<T>,
                                     const std::remove_const_t<U>,
                                     std::remove_const_t<U>>
{
};
/**
 * @brief helper alias template: copy_const
 * @tparam T 
 * @tparam U 
 */
template <class T, class U>
using copy_const_t = typename copy_const<T, U>::type;

} // namespace rib::type