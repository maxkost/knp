/**
 * @file type.h
 * @brief I/O type tag.
 * @author Artiom N.
 * @date 10.06.2024
 */

#pragma once

/**
 * @brief Input channel namespace.
 */
namespace knp::framework::io
{
/**
 * @brief I/O type tag name.
 */
constexpr char io_type_tag[]{"io_type"};


/**
 * @brief I/O type tag values.
 */
enum class IOType
{
    input = 0,
    output = 1
};

}  // namespace knp::framework::io
