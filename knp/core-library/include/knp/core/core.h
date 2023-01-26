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


namespace knp::core
{

/**
 * @brief Tags.
 */
class TagMap
{
public:
    [[nodiscard]] std::any &get_tag(const std::string &name) { return tags_[name]; }
    template <typename T>
    [[nodiscard]] std::any &get_tag(const std::string &name)
    {
        return std::any_cast<T>(tags_[name]);
    }

private:
    std::map<std::string, std::any> tags_;
};


/**
 * @brief Common data for the several different components.
 */
struct BaseData
{
    UID uid_;
    TagMap tags_;
};
}  // namespace knp::core
