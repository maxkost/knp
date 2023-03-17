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
 * @brief The Device class is the base class for devices supported by the device library.
 */
class Device
{
public:
    /**
     * @brief Get the device UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * @brief Get tags used by the device.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Get the device type.
     * @return device type.
     */
    [[nodiscard]] virtual DeviceType get_type() const = 0;

    /**
     * @brief Get the device name.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] virtual const std::string &get_name() const = 0;

    /**
     * @brief Get the power consumption details for the device.
     * @return amount of consumed power.
     */
    [[nodiscard]] virtual const float get_power() const = 0;

protected:
    BaseData base_;
};

}  // namespace knp::core
