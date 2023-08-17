/**
 * @file specialization_helpers.h
 * @brief Some useful routines working with template specializations/
 * @author Artiom N.
 * @date 11.08.2023
 */

#pragma once

#include <type_traits>


/**
 * @brief Metaprogramming library namespace.
 */
namespace knp::meta
{

/**
 * @brief Specialization test template.
 * @note https://stackoverflow.com/questions/31762958/check-if-class-is-a-template-specialization
 * @tparam T possibly specialization.
 * @code
    static_assert(is_specialization<std::vector<int>, std::vector>{}, "");
    static_assert(!is_specialization<std::vector<int>, std::list>{}, "");
 * @endcode
 */
template <class T, template <class...> class Template>
struct is_specialization : std::false_type
{
};


template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type
{
};

}  // namespace knp::meta
