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

#include <exception>
#include <utility>


namespace knp::devices::cpu
{

void check_pcm_status(const pcm::PCM::ErrorCode& status)
{
    switch (status)
    {
        case pcm::PCM::Success:
        {
            SPDLOG_TRACE("PCM instance programming is ok");
            break;
        }
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
}


CpuPower::CpuPower(uint32_t cpu_sock_no) : cpu_sock_no_(cpu_sock_no), pcm_instance_(pcm::PCM::getInstance())
{
    // This is the instance, memory releasing doesn't need.
    const pcm::PCM::ErrorCode status = pcm_instance_->program(pcm::PCM::DEFAULT_EVENTS, nullptr, true, ::getpid());
    check_pcm_status(status);

    try
    {
        get_power();
    }
    catch (const std::logic_error& e)
    {
        SPDLOG_WARN("Error during power consumption counter init: {}", e.what());
    }
}


float CpuPower::get_power()
{
    if (!pcm_instance_->packageEnergyMetricsAvailable())
    {
        throw std::logic_error("Energy metrics unavailable!");
    }

    std::swap(sktstate1_, sktstate2_);

    sktstate1_ = pcm_instance_->PCM::getSocketCounterState(cpu_sock_no_);
    // pcm_instance_->getAllCounterStates(sstate2_, sktstate2_, cstates2_);
    const auto time_now = std::chrono::steady_clock::now();
    const auto consumed_joules = pcm::getConsumedJoules(sktstate2_, sktstate1_);
    const auto consumed_watts =
        consumed_joules / std::chrono::duration_cast<std::chrono::seconds>(time_now - time_start_).count();

    SPDLOG_DEBUG("CPU, Joules = {}, Watts = {}", consumed_joules, consumed_watts);

    time_start_ = time_now;

    return consumed_watts;
}
}  // namespace knp::devices::cpu
