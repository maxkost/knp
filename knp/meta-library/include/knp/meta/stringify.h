/**
 * @file stringify.h
 * @brief Introspection routines.
 * @author Artiom N.
 * @date 10.05.2023
 */

#pragma once

#include <string>
#include <tuple>
#include <vector>

#include <boost/mp11.hpp>
#include <boost/preprocessor.hpp>


/**
 * @brief Metaprogramming library namespace.
 */
namespace knp::meta
{

/**
 * @brief Template "not equal" function.
 * @tparam T1 first type to compare.
 * @tparam T2 second type to compare.
 */
template <class T1, class T2>
using mp_neq = boost::mp11::mp_bool<T1::value != T2::value>;

/**
 * @brief Get types from the global list only contained in the local list.
 * @tparam AllList global types list.
 * @tparam L local types list.
 * @result types list.
 */
template <class AllList, class L>
using mp_flt = boost::mp11::mp_filter_q<boost::mp11::mp_bind_front<mp_neq, boost::mp11::mp_size<AllList>>, L>;

/**
 * @brief Get type indexes from the global list which contained in the local list.
 * @tparam AllList global types list.
 * @tparam L local types list.
 * @result indexes array.
 */
template <class AllList, class L>
using mp_supported_indexes =
    mp_flt<AllList, boost::mp11::mp_transform_q<boost::mp11::mp_bind_front<boost::mp11::mp_find, AllList>, L>>;

/**
 * @brief Internal macro.
 */
#define KNP_MAKE_TUPLE_INTERNAL(n, tuple_type, tuple_elem) (BOOST_PP_STRINGIZE(tuple_elem))

/**
 * @brief Make tuple from the passed elements.
 */
#define KNP_MAKE_TUPLE(tuple_elems) \
    ::std::make_tuple(              \
        BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FOR_EACH(KNP_MAKE_TUPLE_INTERNAL, "", BOOST_PP_VARIADIC_TO_SEQ(tuple_elems))));

/**
 * @brief Return supported type names (i.e. of the neurons and synapses).
 * @tparam AllTypes global types list.
 * @tparam SupportedTypes local types list.
 * @tparam Names container type. Doesn't necessary. Will be derived automatically.
 * @result vector of strings.
 */
template <typename AllTypes, typename SupportedTypes, typename NamesType>
std::vector<std::string> get_supported_type_names(const NamesType &type_names)
{
    std::vector<std::string> result;
    result.reserve(boost::mp11::mp_size<SupportedTypes>::value);

    boost::mp11::mp_for_each<knp::meta::mp_supported_indexes<SupportedTypes, AllTypes>>(
        [&type_names, &result](auto i) { result.push_back(std::get<i>(type_names)); });

    return result;
}

}  // namespace knp::meta
