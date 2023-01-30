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


/// Core library namespace.
namespace knp::core
{

/**
 * @brief Tags.
 */
class TagMap
{
public:
    /**
     * Get tag by name.
     * @param name tag name.
     * @return tag value.
     */
    [[nodiscard]] std::any &get_tag(const std::string &name) { return tags_[name]; }

    /**
     * Get typed tag by name.
     * @tparam tag value type.
     * @param name tag name.
     * @return tag value.
     */
    template <typename T>
    [[nodiscard]] std::any &get_tag(const std::string &name)
    {
        return std::any_cast<T>(tags_[name]);
    }

private:
    std::map<std::string, std::any> tags_;
};


/**
 * @brief Common data for the several different entities.
 * @see Backend has BaseData.
 * @see Device has BaseData.
 * @see Population has BaseData.
 * @see Projection has BaseData.
 */
struct BaseData
{
    /// Entity unique identifier.
    UID uid_;
    /// Entity tags.
    TagMap tags_;
};
}  // namespace knp::core
