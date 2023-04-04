/**
 * @file cpu.h
 * @brief CPU device class definition.
 * @author Artiom N.
 * @date 30.01.2023
 */

#pragma once

#include <knp/core/device.h>

#include <memory>
#include <string>
#include <vector>


namespace knp::devices::cpu
{

/**
* @brief The CpuPower class stores the power counter monitor (PCM) instance that monitors the state of a processor.
*/
class CpuPower;

class CPU : public knp::core::Device
{
public:
    CPU();
    ~CPU();

    CPU(const CPU &) = delete;

    CPU(CPU &&);
    CPU &operator=(CPU &&);

public:
    /**
     * @brief Get the device type.
     * @return device type.
     */
    [[nodiscard]] knp::core::DeviceType get_type() const override;

    /**
     * @brief Get the device name.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] const std::string &get_name() const override;

    /**
     * @brief Get the power consumption details for the device.
     * @return amount of consumed power.
     */
    [[nodiscard]] float get_power() const override;

private:
    // Non const, because of move operator.
    std::string cpu_name_;
    mutable std::unique_ptr<CpuPower> power_meter_;
};


std::vector<CPU> list_processors();

}  // namespace knp::devices::cpu
