/**
 * @file assert_helpers.h
 * @brief Some helpers that are useful for assertions.
 * @author Artiom N.
 * @date 31.08.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once


/**
 * @brief Metaprogramming library namespace.
 */
namespace knp::meta
{

/**
 * @brief Template used to generate a fixed `false` value for static asserts.
 */
template <class>
inline constexpr bool always_false_v = false;

}  // namespace knp::meta
