/**
 * @file network.h
 * @brief Network interface.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/neuron-traits/all_traits.h>
#include <knp/synapse-traits/all_traits.h>

#include <vector>


/// Framework namespace.
namespace knp::framework
{

/**
 * @brief The Network class is a neural network containing populations and projections.
 */
class Network
{
public:
    using AllPopulations = boost::mp11::mp_transform<knp::core::Population, knp::neuron_traits::AllNeurons>;
    using AllProjections = boost::mp11::mp_transform<knp::core::Projection, knp::synapse_traits::AllSynapses>;

    using AllPopulationVariants = boost::mp11::mp_rename<AllPopulations, std::variant>;
    using AllProjectionVariants = boost::mp11::mp_rename<AllProjections, std::variant>;

public:
private:
    std::vector<AllPopulationVariants> populations_;
    std::vector<AllProjectionVariants> projections_;
};

}  // namespace knp::framework
