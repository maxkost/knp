/**
 * @file population.h
 * @brief General Population Interface.
 * @author Artiom N.
 * @date 18.01.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/messaging/synaptic_impact_message.h>
#include <knp/core/uid.h>
#include <knp/neuron-traits/type_traits.h>

#include <functional>
#include <vector>


namespace knp::core
{


/**
 * @brief The Population class is a container of neurons of the same model.
 * @tparam NeuronType type of the population neurons.
 * @see Neuron.
 */
template <typename NeuronType>
class Population
{
public:
    using NeuronGenerator = std::function<neuron_traits::neuron_parameters<NeuronType> && (Population<NeuronType> &)>;

public:
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    [[nodiscard]] std::vector<neuron_traits::neuron_parameters<NeuronType>> get_neurons_parameters() const;
    [[nodiscard]] neuron_traits::neuron_parameters<NeuronType> get_neurons_parameters(size_t index) const;
    void set_neurons_parameters(size_t index, neuron_traits::neuron_parameters<NeuronType> &&parameters);
    void set_neurons_parameters(size_t index, const neuron_traits::neuron_parameters<NeuronType> &parameters);

public:
    [[nodiscard]] const TagMap &get_neuron_tags(size_t index) const;

public:
    void add_neurons(NeuronGenerator generator);
    void remove_neurons(const std::vector<size_t> &neuron_indexes);

public:
    void reset_weights();
    void lock_weights();
    void unlock_weights();

public:
    void operator()(const messaging::SynapticImpactMessage &synaptic_impact);

public:
    UID presynaptic_population_uid_;
    UID postsynaptic_population_uid_;
    BaseData base_;
};

}  // namespace knp::core
