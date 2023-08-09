/**
 * @file backend.cpp
 * @brief Multi-threaded CPU backend class implementation.
 * @author Artiom N.
 * @date 21.06.2023
 */


#include <knp/backends/cpu-library/blifat_population.h>
#include <knp/backends/cpu-library/delta_synapse_projection.h>
#include <knp/backends/cpu-multi-threaded/backend.h>
#include <knp/core/core.h>
#include <knp/devices/cpu.h>
#include <knp/meta/stringify.h>
#include <knp/meta/variant_helpers.h>

#include <spdlog/spdlog.h>

#include <functional>
#include <optional>
#include <vector>

#include <boost/mp11.hpp>


namespace knp::backends::multi_threaded_cpu
{


MultiThreadedCPUBackend::MultiThreadedCPUBackend(size_t thread_count)
    : message_endpoint_{message_bus_.create_endpoint()}, calc_pool_{thread_count}
{
    SPDLOG_INFO("MT CPU backend instance created, threads count = {}...", thread_count);
}


MultiThreadedCPUBackend::~MultiThreadedCPUBackend()
{
    // stop() and join() will be called in the thread_pool desctructor.
}


std::shared_ptr<MultiThreadedCPUBackend> MultiThreadedCPUBackend::create()
{
    SPDLOG_DEBUG("Creating MT CPU backend instance...");
    return std::make_shared<MultiThreadedCPUBackend>();
}


std::vector<std::string> MultiThreadedCPUBackend::get_supported_neurons() const
{
    return knp::meta::get_supported_type_names<knp::neuron_traits::AllNeurons, SupportedNeurons>(
        knp::neuron_traits::neurons_names);
}


std::vector<std::string> MultiThreadedCPUBackend::get_supported_synapses() const
{
    return knp::meta::get_supported_type_names<knp::synapse_traits::AllSynapses, SupportedSynapses>(
        knp::synapse_traits::synapses_names);
}


std::vector<size_t> MultiThreadedCPUBackend::get_supported_projection_indexes() const
{
    return knp::meta::get_supported_type_indexes<core::AllProjections, SupportedProjections>();
}


std::vector<size_t> MultiThreadedCPUBackend::get_supported_population_indexes() const
{
    return knp::meta::get_supported_type_indexes<core::AllPopulations, SupportedPopulations>();
}


void MultiThreadedCPUBackend::step()
{
    SPDLOG_DEBUG("Starting step #{}", get_step());
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
                    static_assert(knp::core::always_false_v<T>, "Population isn't supported by the CPU MT backend!");
                boost::asio::post(
                    calc_pool_,
                    [this, &arg]() { std::invoke(&MultiThreadedCPUBackend::calculate_population, this, arg); });
            },
            e);
    }

    //    calc_pool_.join();

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
                    static_assert(knp::core::always_false_v<T>, "Projection isn't supported by the CPU MT backend!");
                // Quick and dirty.
                boost::asio::post(
                    calc_pool_, [this, &arg, &e]()
                    { std::invoke(&MultiThreadedCPUBackend::calculate_projection, this, arg, e.messages_); });
            },
            e.arg_);
    }

    //    calc_pool_.join();
    message_bus_.route_messages();
    message_endpoint_.receive_all_messages();

    auto step = gad_step();
    SPDLOG_DEBUG("Step finished #{}", step);
}


void MultiThreadedCPUBackend::load_populations(const std::vector<PopulationVariants> &populations)
{
    SPDLOG_DEBUG("Loading populations");
    populations_.clear();
    populations_.reserve(populations.size());

    for (const auto &p : populations) populations_.push_back(p);
    SPDLOG_DEBUG("All populations loaded");
}


void MultiThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &projections)
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


void MultiThreadedCPUBackend::load_all_projections(const std::vector<knp::core::AllProjectionsVariant> &projections)
{
    SPDLOG_DEBUG("Loading all projections");
    knp::meta::load_from_container<SupportedProjections>(projections, projections_);
    SPDLOG_DEBUG("All projections loaded");
}


void MultiThreadedCPUBackend::load_all_populations(const std::vector<knp::core::AllPopulationsVariant> &populations)
{
    SPDLOG_DEBUG("Loading all populations");
    knp::meta::load_from_container<SupportedPopulations>(populations, populations_);
    SPDLOG_DEBUG("All populations loaded");
}


std::vector<std::unique_ptr<knp::core::Device>> MultiThreadedCPUBackend::get_devices() const
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


void MultiThreadedCPUBackend::init()
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


void MultiThreadedCPUBackend::calculate_population(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population)
{
    SPDLOG_TRACE("Calculate population {}", std::string(population.get_uid()));
    calculate_blifat_population(population, message_endpoint_, get_step(), ep_mutex_);
}


void MultiThreadedCPUBackend::calculate_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection,
    core::messaging::SynapticMessageQueue &message_queue)
{
    SPDLOG_TRACE("Calculate projection {}", std::string(projection.get_uid()));
    calculate_delta_synapse_projection(projection, message_endpoint_, message_queue, get_step(), ep_mutex_);
}


MultiThreadedCPUBackend::PopulationIterator MultiThreadedCPUBackend::begin_populations()
{
    return populations_.begin();
}


MultiThreadedCPUBackend::PopulationConstIterator MultiThreadedCPUBackend::begin_populations() const
{
    return populations_.cbegin();
}


MultiThreadedCPUBackend::PopulationIterator MultiThreadedCPUBackend::end_populations()
{
    return populations_.end();
}


MultiThreadedCPUBackend::PopulationConstIterator MultiThreadedCPUBackend::end_populations() const
{
    return populations_.cend();
}


MultiThreadedCPUBackend::ProjectionIterator MultiThreadedCPUBackend::begin_projections()
{
    return projections_.begin();
}


MultiThreadedCPUBackend::ProjectionConstIterator MultiThreadedCPUBackend::begin_projections() const
{
    return projections_.cbegin();
}


MultiThreadedCPUBackend::ProjectionIterator MultiThreadedCPUBackend::end_projections()
{
    return projections_.end();
}


MultiThreadedCPUBackend::ProjectionConstIterator MultiThreadedCPUBackend::end_projections() const
{
    return projections_.cend();
}


}  // namespace knp::backends::multi_threaded_cpu
