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
    /**
     * Neurons generator type.
     */
    using NeuronGenerator =
        std::function<neuron_traits::neuron_parameters<NeuronType> && (Population<NeuronType> &, size_t index)>;

public:
    /**
     * Get this population UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * Get this population tags.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * Get parameters of the all neurons in the population.
     * @return parameters vector.
     */
    [[nodiscard]] std::vector<neuron_traits::neuron_parameters<NeuronType>> get_neurons_parameters() const;

    /**
     * Get parameters of the concrete neuron in the population.
     * @param index neuron index.
     * @return parameters vector.
     */
    [[nodiscard]] neuron_traits::neuron_parameters<NeuronType> get_neurons_parameters(size_t index) const;

    /**
     * Set parameters of the concrete neuron in the population.
     * @param index neuron index.
     * @param parameters neuron parameters.
     */
    void set_neurons_parameters(size_t index, neuron_traits::neuron_parameters<NeuronType> &&parameters);

    /**
     * Set parameters of the concrete neuron in the population.
     * @param index neuron index.
     * @param parameters neuron parameters.
     */
    void set_neurons_parameters(size_t index, const neuron_traits::neuron_parameters<NeuronType> &parameters);

public:
    /**
     * Get concrete neuron tags.
     * @param index neuron index.
     * @return neuron tags.
     */
    [[nodiscard]] const TagMap &get_neuron_tags(size_t index) const;

public:
    /**
     * Add neurons to the population.
     * @param generator neurons adding strategy.
     */
    void add_neurons(NeuronGenerator generator);

    /**
     * Remove neurons from the population.
     * @param neuron_indexes indexes of the removing neurons.
     */
    void remove_neurons(const std::vector<size_t> &neuron_indexes);

public:
    void operator()(const messaging::SynapticImpactMessage &synaptic_impact);

public:
    UID presynaptic_population_uid_;
    UID postsynaptic_population_uid_;
    BaseData base_;
};

}  // namespace knp::core
