/**
 * @file cpu.cpp
 * @brief CPU device class implementation.
 * @author Artiom N.
 * @date 30.01.2023
 */


#include <knp/devices/cpu.h>

#include "cpu_power.h"


namespace knp::devices::cpu
{

CPU::CPU() : power_meter_{std::make_unique<CpuPower>()} {}
CPU::~CPU() {}
CPU::CPU(CPU&& other) : cpu_name_{std::move(other.cpu_name_)}, power_meter_{std::move(other.power_meter_)} {}


CPU& CPU::operator=(CPU&& other) noexcept
{
    cpu_name_.swap(other.cpu_name_);
    power_meter_.swap(other.power_meter_);
    return *this;
}


knp::core::DeviceType CPU::get_type() const
{
    return knp::core::DeviceType::CPU;
}


const std::string& CPU::get_name() const
{
    return cpu_name_;
}


float CPU::get_power() const
{
    return power_meter_->get_power();
}


std::vector<CPU> list_processors()
{
    return std::vector<CPU>();
}

}  // namespace knp::devices::cpu
