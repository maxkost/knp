#pragma once

/**
 * @brief Core library main include.
 * @author Artiom N.
 * @date 11.01.2023
 */

#include <knp/uid.h>

#include <any>
#include <map>
#include <string>


namespace knp::core
{

/**
 * Tags.
 */
class TagMap
{
public:
    [[nodiscard]] get_tag(const std::string &name)->&std::any { return tags_[name]; }
    template <typename T>
    [[nodiscard]] get_tag<T>(const std::string &name)->std::any
    {
        return std::any_cast<T> tags_[name];
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
