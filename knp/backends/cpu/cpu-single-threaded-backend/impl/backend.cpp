/**
 * @file backend.cpp
 * @brief Single-threaded CPU backend class implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */


#include <knp/backends/cpu-library/blifat_population.h>
#include <knp/backends/cpu-library/delta_synapse_projection.h>
#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/core/core.h>
#include <knp/devices/cpu.h>
#include <knp/meta/stringify.h>

#include <spdlog/spdlog.h>

#include <functional>
#include <vector>

#include <boost/mp11.hpp>


namespace knp::backends::single_threaded_cpu
{

// template <typename TypeList, auto CalculateMethod, typename Container>
// inline void SingleThreadedCPUBackend::calculator(Container &container)
//{
//     for (auto &e : container)
//     {
//         std::visit(
//             [this, &e](auto &arg)
//             {
//                 using T = std::decay_t<decltype(arg)>;
//                 if constexpr (boost::mp11::mp_find<TypeList, T>{} == boost::mp11::mp_size<TypeList>{})
//                     static_assert(knp::core::always_false_v<T>, "Type isn't supported by the CPU ST backend!");
//                 std::invoke(CalculateMethod, this, arg, e);
//             },
//             e.arg_);
//     }
// }


SingleThreadedCPUBackend::SingleThreadedCPUBackend() : message_endpoint_{message_bus_.create_endpoint()}
{
    SPDLOG_INFO("ST CPU backend instance created...");
}


std::shared_ptr<SingleThreadedCPUBackend> SingleThreadedCPUBackend::create()
{
    SPDLOG_DEBUG("Creating ST CPU backend instance...");
    return std::make_shared<SingleThreadedCPUBackend>();
}


std::vector<std::string> SingleThreadedCPUBackend::get_supported_neurons() const
{
    return knp::meta::get_supported_type_names<knp::neuron_traits::AllNeurons, SupportedNeurons>(
        knp::neuron_traits::neurons_names);
}


std::vector<std::string> SingleThreadedCPUBackend::get_supported_synapses() const
{
    return knp::meta::get_supported_type_names<knp::synapse_traits::AllSynapses, SupportedSynapses>(
        knp::synapse_traits::synapses_names);
}


void SingleThreadedCPUBackend::step()
{
    SPDLOG_DEBUG(std::string("Starting step #") + std::to_string(step_));
    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();
    // Calculate populations.
    for (auto &e : populations_)
    {
        std::visit(
            [this](auto &arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (
                    boost::mp11::mp_find<SupportedPopulations, T>{} == boost::mp11::mp_size<SupportedPopulations>{})
                    static_assert(knp::core::always_false_v<T>, "Population isn't supported by the CPU ST backend!");
                std::invoke(&SingleThreadedCPUBackend::calculate_population, this, arg);
            },
            e);
    }

    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();
    // Calculate projections.
    for (auto &e : projections_)
    {
        std::visit(
            [this, &e](auto &arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (
                    boost::mp11::mp_find<SupportedProjections, T>{} == boost::mp11::mp_size<SupportedProjections>{})
                    static_assert(knp::core::always_false_v<T>, "Projection isn't supported by the CPU ST backend!");
                std::invoke(&SingleThreadedCPUBackend::calculate_projection, this, arg, e.messages_);
            },
            e.arg_);
    }

    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();
    ++step_;
    SPDLOG_DEBUG("Step finished");
}


void SingleThreadedCPUBackend::load_populations(const std::vector<PopulationVariants> &populations)
{
    SPDLOG_DEBUG("Loading populations");
    populations_.clear();
    populations_.reserve(populations.size());

    for (const auto &p : populations) populations_.push_back(p);
    SPDLOG_DEBUG("All populations loaded");
}


void SingleThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &projections)
{
    SPDLOG_DEBUG("Loading projections");
    projections_.clear();
    projections_.reserve(projections.size());

    for (const auto &p : projections)
    {
        projections_.push_back(ProjectionWrapper{p});
    }

    SPDLOG_DEBUG("All projections loaded");
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


void SingleThreadedCPUBackend::init()
{
    SPDLOG_DEBUG("Initializing...");

    for (const auto &p : projections_)
    {
        const auto [pre_uid, post_uid, this_uid] = std::visit(
            [](const auto &proj)
            { return std::make_tuple(proj.get_presynaptic(), proj.get_postsynaptic(), proj.get_uid()); },
            p.arg_);

        if (pre_uid) message_endpoint_.subscribe<knp::core::messaging::SpikeMessage>(this_uid, {pre_uid});
        if (post_uid) message_endpoint_.subscribe<knp::core::messaging::SynapticImpactMessage>(post_uid, {this_uid});
    }
    SPDLOG_DEBUG("Initializing finished...");
}


void SingleThreadedCPUBackend::calculate_population(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population)
{
    SPDLOG_TRACE(std::string("Calculate population") + std::string(population.get_uid()));
    calculate_blifat_population(population, message_endpoint_, step_);
}


void SingleThreadedCPUBackend::calculate_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection,
    core::messaging::SynapticMessageQueue &message_queue)
{
    SPDLOG_TRACE(std::string("Calculate projection ") + std::string(projection.get_uid()));
    calculate_delta_synapse_projection(projection, message_endpoint_, message_queue, step_);
}


SingleThreadedCPUBackend::PopulationIterator SingleThreadedCPUBackend::begin_populations()
{
    return populations_.begin();
}


SingleThreadedCPUBackend::PopulationConstIterator SingleThreadedCPUBackend::begin_populations() const
{
    return populations_.cbegin();
}


SingleThreadedCPUBackend::PopulationIterator SingleThreadedCPUBackend::end_populations()
{
    return populations_.end();
}


SingleThreadedCPUBackend::PopulationConstIterator SingleThreadedCPUBackend::end_populations() const
{
    return populations_.cend();
}


SingleThreadedCPUBackend::ProjectionIterator SingleThreadedCPUBackend::begin_projections()
{
    return projections_.begin();
}


SingleThreadedCPUBackend::ProjectionConstIterator SingleThreadedCPUBackend::begin_projections() const
{
    return projections_.cbegin();
}


SingleThreadedCPUBackend::ProjectionIterator SingleThreadedCPUBackend::end_projections()
{
    return projections_.end();
}


SingleThreadedCPUBackend::ProjectionConstIterator SingleThreadedCPUBackend::end_projections() const
{
    return projections_.cend();
}


}  // namespace knp::backends::single_threaded_cpu
