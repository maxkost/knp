/**
 * @file generator.h
 * @brief Coordinates generator.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <any>
#include <functional>


/**
 * @brief Namespace for framework coordinates.
 */
namespace knp::framework::coordinates
{
/**
 * @brief Type of coordinate generator defined by a functor that returns a random value out of the specified range.
 * @param index value defining a range of possible coordinate values.
*/
using CoordinateGenerator = std::function<std::any(size_t index)>;

}  // namespace knp::framework::coordinates
