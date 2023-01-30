/**
 * @file device.h
 * @brief Device base class definition.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <knp/core/core.h>

#include <string>


namespace knp::core
{

enum class DeviceType
{
    CPU,
    GPU,
    AltAI1_NPU
};

/**
 * @brief Device base class.
 */
class Device
{
public:
    /**
     * Get this device UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * Get this device tags.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * Get current device type.
     * @return device type.
     */
    [[nodiscard]] virtual DeviceType get_type() const = 0;

    /**
     * Get device name, i.e. CPU0.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] virtual const std::string &get_name() const = 0;

    /**
     * Get device power consumption.
     * @return power consumption.
     */
    [[nodiscard]] virtual const float &get_power() const = 0;

private:
    BaseData base_;
};

}  // namespace knp::core
