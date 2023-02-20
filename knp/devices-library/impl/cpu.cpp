/**
 * @file cpu.cpp
 * @brief CPU device class implementation.
 * @author Artiom N.
 * @date 30.01.2023
 */


#include <knp/devices/cpu.h>

#include "cpu_power.h"


namespace knp::devices
{

CPU::CPU() : power_meter_{std::make_unique<CpuPower>()} {}


knp::core::DeviceType CPU::get_type() const
{
    return knp::core::DeviceType::CPU;
}


const std::string &CPU::get_name() const
{
    return "CPU0";
}


const float CPU::get_power() const
{
    return power_meter_->get_power();
}
}  // namespace knp::devices
