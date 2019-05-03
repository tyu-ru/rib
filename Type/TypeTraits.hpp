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

/**
 * @brief type to which the member belong
 */
template <class>
struct belong_type
{
    using type = void;
};
template <class T, class U>
struct belong_type<U T::*>
{
    using type = T;
};
template <class R, class T, class... Args>
struct belong_type<R (T::*)(Args...)>
{
    using type = T;
};
template <class T>
using belong_type_t = typename belong_type<T>::type;

static_assert(std::is_same_v<belong_type_t<int(int)>, void>);

/**
 * @brief strict invoke result type check
 * @tparam R result type
 * @tparam F invokeable type
 * @tparam Args 
 */
template <class R, class F, class... Args>
struct is_result
    : std::is_same<
          R,
          typename std::conditional_t<
              std::is_member_function_pointer_v<F> || std::is_member_object_pointer_v<F>,
              std::invoke_result<F, belong_type_t<F>, Args...>,
              std::invoke_result<F, Args...>>::type>
{
};
template <class R, class F, class... Args>
constexpr bool is_result_v = is_result<R, F, Args...>::value;

static_assert(is_result_v<int, int(int), int>);
static_assert(!is_result_v<int, long(int), int>);

#define DECLV(T) std::declval<T>()
#define RESULT_T(T, F) decltype(DECLV(T).F)

} // namespace rib::type
