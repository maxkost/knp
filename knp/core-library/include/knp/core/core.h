/**
 * @file core.h
 * @brief Core library main include.
 * @author Artiom N.
 * @date 11.01.2023
 */

#pragma once

#include <knp/core/uid.h>

#include <any>
#include <map>
#include <string>
#include <variant>


/**
 * @brief Core library namespace.
 */
namespace knp::core
{
/**
 * @brief Template used to generate a fixed `false` value for static asserts.
 */
template <class>
inline constexpr bool always_false_v = false;

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
    [[nodiscard]] T &get_tag(const std::string &name)
    {
        return std::any_cast<T>(tags_[name]);
    }

private:
    std::map<std::string, std::any> tags_;
};


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
};

}  // namespace knp::core
