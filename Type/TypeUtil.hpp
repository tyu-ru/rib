#pragma once
#include <utility>

namespace rib::type
{

template <class T>
struct TypeAdapter
{
    using type = T;
};

template <template <class...> class T>
struct TemplateAdaperByType
{
    template <class... Args>
    using Instance = T<Args...>;
    template <class... Args>
    using TypeAdapter = TypeAdapter<T<Args...>>;
};
template <template <auto...> class T>
struct TemplateAdaperByValue
{
    template <auto... Args>
    using Instance = T<Args...>;
    template <auto... Args>
    using TypeAdapter = TypeAdapter<T<Args...>>;
};

template <template <class...> class Template, class Instance>
struct is_template_instance_by_type : std::false_type
{
};
template <template <class...> class Template, class... Args>
struct is_template_instance_by_type<Template, Template<Args...>> : std::true_type
{
};
template <template <class...> class Template, class Instance>
constexpr bool is_template_instance_by_type_v = is_template_instance_by_type<Template, Instance>::value;

static_assert(!is_template_instance_by_type_v<TypeAdapter, int>);
static_assert(is_template_instance_by_type_v<TypeAdapter, TypeAdapter<int>>);

template <template <auto...> class Template, class Instance>
struct is_template_instance_by_value : std::false_type
{
};
template <template <auto...> class Template, auto... Args>
struct is_template_instance_by_value<Template, Template<Args...>> : std::true_type
{
};
template <template <auto...> class Template, class Instance>
constexpr bool is_template_instance_by_value_v = is_template_instance_by_value<Template, Instance>::value;

namespace test
{

template <int x>
struct INT
{
};

static_assert(!is_template_instance_by_value_v<INT, int>);
static_assert(is_template_instance_by_value_v<INT, INT<0>>);

} // namespace test
} // namespace rib::type
