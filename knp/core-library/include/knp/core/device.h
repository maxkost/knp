/**
 * @file device.h
 * @brief Device base class definition.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

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
    virtual DeviceType get_device_type() const = 0;

private:
    BaseData base_;
};

}  // namespace knp::core
