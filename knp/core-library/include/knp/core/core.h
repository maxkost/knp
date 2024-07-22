/**
 * @file core.h
 * @brief Class definition for core library basic entities.
 * @author Artiom N.
 * @date 11.01.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/tag.h>
#include <knp/core/uid.h>


/**
 * @namespace knp
 * @brief General framework namespace.
 */


/**
 * @brief Core library namespace.
 */
namespace knp::core
{
/**
 * @brief Common parameters for several different entities.
 * @details For example, BaseData is used in entities of Backend, Device, Population and Projection classes.
 */
struct BaseData
{
    /**
     * @brief Entity unique identifier.
     */
    UID uid_;
    /**
     * @brief Entity tags.
     */
    TagMap tags_;
}
#if defined(__GNUC__) || defined(__clang__)
/**
 * @brief Attribute for alignment.
 * @return none.
 */
__attribute__((aligned(sizeof(size_t) * 8)));
#else
;  // NOLINT
#endif


/**
 * @brief Type used to store a step number in the form of a 64-bit unsigned integer.
 */
using Step = uint64_t;

}  // namespace knp::core
