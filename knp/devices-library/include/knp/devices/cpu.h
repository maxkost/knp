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


/**
 * @brief Device namespace.
*/
namespace knp::devices::cpu
{

/**
* @brief The CpuPower class stores the power counter monitor (PCM) instance that monitors the state of a processor.
*/
class CpuPower;

/**
 * @brief The CPU class is a definition of an interface to the CPU device.
*/
class CPU : public knp::core::Device
{
public:

    /**
     * @brief CPU device constructor.
    */
    CPU();

    /**
     * @brief CPU device destructor.
    */
    ~CPU();

    /**
     * @brief Avoid copy assignment of a CPU device.
    */
    CPU(const CPU &) = delete;

    /**
     * @brief CPU device move constructor.
     */
    CPU(CPU &&);

    /**
     * @brief CPU device copy operator.
    */
    CPU &operator=(CPU &&);

public:
    /**
     * @brief Get device type.
     * @return device type.
     */
    [[nodiscard]] knp::core::DeviceType get_type() const override;

    /**
     * @brief Get device name.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] const std::string &get_name() const override;

    /**
     * @brief Get power consumption details for the device.
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
