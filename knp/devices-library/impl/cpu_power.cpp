/**
 * @file cpu_power.cpp
 * @brief CPU device class implementation.
 * @author Artiom N.
 * @date 30.01.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "cpu_power.h"

#include <pcm/src/utils.h>
#include <spdlog/spdlog.h>

#include <utility>


namespace knp::devices::cpu
{
CpuPower::CpuPower()
{
    // This is the instance, memory releasing doesn't need.
    pcm_instance_ = pcm::PCM::getInstance();
    int pid = ::getpid();

    const pcm::PCM::ErrorCode status = pcm_instance_->program(pcm::PCM::DEFAULT_EVENTS, nullptr, false, pid);

    switch (status)
    {
        case pcm::PCM::Success:
            break;
        case pcm::PCM::MSRAccessDenied:
            throw std::logic_error(
                "Access to Intel(r) Performance Counter Monitor has denied (no MSR or PCI CFG space access).");
        case pcm::PCM::PMUBusy:
            throw std::logic_error(
                "Access to Intel(r) Performance Counter Monitor has denied (Performance Monitoring Unit is occupied by "
                "other application). Try to stop the application that uses PMU.");
        default:
            throw std::logic_error("Access to Intel(r) Performance Counter Monitor has denied (Unknown error).");
    }

    get_power();
}


float CpuPower::get_power()
{
    pcm_instance_->getAllCounterStates(sstate2_, sktstate2_, cstates2_);
    const auto time_now = std::chrono::steady_clock::now();

    const auto consumed_watts = pcm::getConsumedJoules(sstate1_, sstate2_) /
                                std::chrono::duration_cast<std::chrono::seconds>(time_now - time_start_).count();

    if (pcm_instance_->packageEnergyMetricsAvailable())
    {
        SPDLOG_INFO("CPU, Watts: {}", consumed_watts);
        for (uint32_t i = 0; i < pcm_instance_->getNumSockets(); ++i)
            SPDLOG_DEBUG("Joules: {}", pcm::getConsumedJoules(sktstate1_[i], sktstate2_[i]));
    }

    std::swap(sstate1_, sstate2_);
    std::swap(sktstate1_, sktstate2_);
    time_start_ = time_now;

    return consumed_watts;
}
}  // namespace knp::devices::cpu
