/**
 * @file cpu.cpp
 * @brief CPU device class implementation.
 * @author Artiom N.
 * @date 30.01.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#include <knp/devices/cpu.h>

#include <pcm/src/cpucounters.h>

#include <boost/uuid/name_generator.hpp>

#include "cpu_power.h"


namespace knp::devices::cpu
{

CPU::CPU(uint32_t cpu_num) : cpu_num_(cpu_num), power_meter_{std::make_unique<CpuPower>(cpu_num)}
{
    auto pcm_instance = pcm::PCM::getInstance();
    const pcm::PCM::ErrorCode status = pcm_instance->program(pcm::PCM::DEFAULT_EVENTS, nullptr, true, ::getpid());
    check_pcm_status(status);

    cpu_name_ = pcm_instance->getCPUBrandString() + " " + pcm_instance->getCPUFamilyModelString() + " " +
                std::to_string(cpu_num);
    Device::base_.uid_ = knp::core::UID(boost::uuids::name_generator(boost::uuids::uuid())(cpu_name_.c_str()));
}


CPU::CPU(CPU&& other) : cpu_name_{std::move(other.cpu_name_)}, power_meter_{std::move(other.power_meter_)} {}


CPU::~CPU() {}


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


uint32_t CPU::get_socket_number() const
{
    return cpu_num_;
}


float CPU::get_power() const
{
    return power_meter_->get_power();
}


std::vector<CPU> list_processors()
{
    auto pcm_instance = pcm::PCM::getInstance();
    const pcm::PCM::ErrorCode status = pcm_instance->program(pcm::PCM::DEFAULT_EVENTS, nullptr, true, ::getpid());

    check_pcm_status(status);

    std::vector<CPU> result;
    result.reserve(pcm_instance->getNumSockets());

    for (uint32_t sock_num = 0; sock_num < pcm_instance->getNumSockets(); ++sock_num)
    {
        if (pcm_instance->isSocketOnline(sock_num)) result.push_back(CPU(sock_num));
    }

    return result;
}

}  // namespace knp::devices::cpu
