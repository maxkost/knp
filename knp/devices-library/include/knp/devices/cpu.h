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

// Need to store PCM instance pointer.
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
     * @brief Get current device type.
     * @return device type.
     */
    [[nodiscard]] knp::core::DeviceType get_type() const override;

    /**
     * @brief Get device name, i.e. CPU0.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] const std::string &get_name() const override;

    /**
     * @brief Get device power consumption.
     * @return power consumption.
     */
    [[nodiscard]] float get_power() const override;

private:
    // Non const, because of move operator.
    std::string cpu_name_;
    mutable std::unique_ptr<CpuPower> power_meter_;
};


std::vector<CPU> list_processors();

}  // namespace knp::devices::cpu
