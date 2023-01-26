/**
 * @file device.h
 * @brief Device base class definition.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

namespace knp::core
{

/**
 * @brief Device base class.
 */
class Device
{
public:
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

private:
    BaseData base_;
};

}  // namespace knp::core
