/**
 * @file tag.h
 * @brief Tag class and well-known tags registry.
 * @kaspersky_support Artiom N.
 * @date 11.06.24.
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <any>
#include <functional>
#include <map>
#include <string>
#include <variant>


/**
 * @brief Core library namespace.
 */
namespace knp::core
{

/**
 * @brief The TagMap class is a definition of tags used by entity and their values.
 */
class TagMap
{
public:
    /**
     * @brief Get tag value by tag name.
     * @param name tag name.
     * @return tag value.
     */
    [[nodiscard]] std::any &get_tag(const std::string &name) { return tags_[name]; }

    /**
     * @brief Get tag value by tag name and value type.
     * @tparam T tag value type.
     * @param name tag name.
     * @return tag value.
     */
    template <typename T>
    [[nodiscard]] std::decay_t<T> &get_tag(const std::string &name)
    {
        return std::any_cast<std::decay_t<T> &>(tags_[name]);
    }

    /**
     * @brief Return tag value.
     * @param name tag name.
     * @return tag value.
     */
    auto &operator[](const std::string &name) { return get_tag(name); }

private:
    std::map<std::string, std::any> tags_{};
};


/**
 * @brief Namespace for known tags.
 */
namespace tags
{
/**
 * @brief Input and output type tag name.
 */
constexpr char io_type_tag[]{"io_type"};


/**
 * @brief Input and output type tag values.
 */
enum class IOType
{
    /**
     * @brief Input tag value.
     */
    input = 0,
    /**
     * @brief Output tag value.
     */
    output = 1
};

}  // namespace tags

}  // namespace knp::core
