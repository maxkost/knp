/**
 * @file backend.cpp
 * @brief Single-threaded CPU backend class implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */


#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/core/core.h>
#include <knp/devices/cpu.h>

#include <spdlog/spdlog.h>

#include <functional>

#include <boost/mp11.hpp>

#include "blifat_population.h"
#include "delta_synapse_projection.h"


namespace knp::backends::single_threaded_cpu
{

template <typename TypeList, auto CalculateMethod, typename Container>
inline void SingleThreadedCPUBackend::calculator(Container &container)
{
    for (auto &e : container)
    {
        std::visit(
            [this, &e](auto &arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (boost::mp11::mp_find<TypeList, T>{} == boost::mp11::mp_size<TypeList>{})
                    static_assert(knp::core::always_false_v<T>, "Type isn't supported by the CPU ST backend!");
                std::invoke(CalculateMethod, this, arg, e);
            },
            e.arg);
    }
}


SingleThreadedCPUBackend::SingleThreadedCPUBackend() : message_endpoint_{message_bus_.get_endpoint()}
{
    SPDLOG_INFO("CPU backend instance created...");
}


std::shared_ptr<SingleThreadedCPUBackend> SingleThreadedCPUBackend::create()
{
    SPDLOG_DEBUG("Creating CPU backend instance...");
    return std::make_shared<SingleThreadedCPUBackend>();
}


void SingleThreadedCPUBackend::step()
{
    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();
    // Calculate populations.
    calculator<SupportedPopulations, &SingleThreadedCPUBackend::calculate_population>(populations_);
    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();
    // Calculate projections.
    calculator<SupportedProjections, &SingleThreadedCPUBackend::calculate_projection>(projections_);
    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();
}


void SingleThreadedCPUBackend::load_populations(const std::vector<PopulationVariants> &populations)
{
    populations_.clear();
    populations_.reserve(populations.size());

    for (const auto &p : populations) populations_.push_back(PopulationWrapper{p});
}


void SingleThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &projections)
{
    projections_.clear();
    projections_.reserve(projections.size());

    for (const auto &p : projections) projections_.push_back(ProjectionWrapper{p});
}


std::vector<std::unique_ptr<knp::core::Device>> SingleThreadedCPUBackend::get_devices() const
{
    std::vector<std::unique_ptr<knp::core::Device>> result;
    auto processors{knp::devices::cpu::list_processors()};

    result.reserve(processors.size());

    for (auto &&cpu : processors)
    {
        SPDLOG_DEBUG("Device CPU \"{}\"", cpu.get_name());
        result.push_back(std::make_unique<knp::devices::cpu::CPU>(std::move(cpu)));
    }

    SPDLOG_DEBUG("CPUs count = {}", result.size());
    return result;
}


void SingleThreadedCPUBackend::calculate_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, PopulationWrapper &wrapper)
{
    calculate_blifat_population(
        std::get<knp::core::Population<neuron_traits::BLIFATNeuron>>(wrapper.arg), message_endpoint_);
}


void SingleThreadedCPUBackend::calculate_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, ProjectionWrapper &wrapper)
{
    calculate_delta_synapse_projection(
        std::get<knp::core::Projection<knp::synapse_traits::DeltaSynapse>>(wrapper.arg), message_endpoint_,
        wrapper.messages, step_);
}
}  // namespace knp::backends::single_threaded_cpu
