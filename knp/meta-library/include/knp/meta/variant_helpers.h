/**
 * @file variant_helpers.h
 * @brief Some useful routines for working with `std::variant`.
 * @author Artiom N.
 * @date 09.08.2023
 */

#pragma once

#include <string>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>


/**
 * @brief Metaprogramming library namespace.
 */
namespace knp::meta
{

/**
 * @brief Load elements from one container of all variants to another container that contains a subset of all
 * variants.
 * @tparam SupportedTypes subset of variants.
 * @tparam AllVariants all supported variants.
 * @tparam ToContainer target container.
 * @param from_container source container.
 * @param to_container target container.
 */
template <typename SupportedTypes, typename AllVariants, typename ToContainer>
void load_from_container(const std::vector<AllVariants> &from_container, ToContainer &to_container)
{
    to_container.clear();
    to_container.reserve(from_container.size());

    for (const auto &p : from_container)
    {
        std::visit(
            [&to_container](auto &arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (boost::mp11::mp_find<SupportedTypes, T>{} != boost::mp11::mp_size<SupportedTypes>{})
                {
                    to_container.push_back(typename ToContainer::value_type{arg});
                }
            },
            p);
    }
}

}  // namespace knp::meta
