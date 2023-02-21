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
#include <utility>
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
    /// Population neuron type.
    using PopulationNeuronType = NeuronType;
    /// Population type, specified with neuron type.
    using PopulationType = Population<NeuronType>;
    /// Neuron parameters type.
    using NeuronParameters = neuron_traits::neuron_parameters<NeuronType>;

    /**
     * @brief Neurons generator type.
     */
    using NeuronGenerator = std::function<NeuronParameters(PopulationType &population, size_t index)>;

public:
    /// Constructor.
    Population(NeuronGenerator generator, size_t neurons_count) { add_neurons(generator, neurons_count); }

public:
    /**
     * @brief Get this population UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * @brief Get this population tags.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Get parameters of the all neurons in the population.
     * @return parameters vector.
     */
    [[nodiscard]] const std::vector<NeuronParameters> &get_neurons_parameters() const { return neurons_; }

    /**
     * @brief Get parameters of the concrete neuron in the population.
     * @param index neuron index.
     * @return parameters vector.
     */
    [[nodiscard]] const NeuronParameters &get_neuron_parameters(size_t index) const { return neurons_[index]; }

    /**
     * @brief Set parameters of the concrete neuron in the population.
     * @param index neuron index.
     * @param parameters neuron parameters.
     */
    void set_neuron_parameters(size_t index, NeuronParameters &&parameters) { neurons_[index] = std::move(parameters); }

    /**
     * @brief Set parameters of the concrete neuron in the population.
     * @param index neuron index.
     * @param parameters neuron parameters.
     */
    void set_neurons_parameters(size_t index, const NeuronParameters &parameters) { neurons_[index] = parameters; }

public:
    /**
     * @brief Get concrete neuron tags.
     * @param index neuron index.
     * @return neuron tags.
     */
    [[nodiscard]] const TagMap &get_neuron_tags(size_t index)
    {
        return base_.tags_.template get_tag<std::vector<TagMap>>("neuron_tags")[index];
    }

public:
    /**
     * @brief Add neurons to the population.
     * @param generator neurons adding strategy.
     */
    void add_neurons(NeuronGenerator generator, size_t count)
    {
        neurons_.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            neurons_.template emplace_back(std::move(generator(*this, i)));
        }
    }

    /**
     * @brief Remove neurons from the population.
     * @param neuron_indexes indexes of the removing neurons.
     */
    void remove_neurons(const std::vector<size_t> &neuron_indexes)
    {
        size_t index_shift = 0;
        for (const auto &index : neuron_indexes)
        {
            remove_neuron(index - index_shift++);
        }
    }

    /**
     * @brief Remove neuron from the population.
     * @param neuron_index index of the removing neuron.
     */
    void remove_neuron(const size_t &neuron_index)
    {
        auto iter = neurons_.begin();
        std::advance(iter, neuron_index);
        neurons_.erase(iter);
    }

public:
    /// Constant neuron indexing operator.
    const auto &operator[](const size_t index) const { return get_neuron_parameters(index); }
    /// Neuron indexing operator.
    auto &operator[](const size_t index) { return neurons_[index]; }
    /// TODO: make iterator class.
    const auto begin() const { return neurons_.cbegin(); }
    /// TODO: make iterator class.
    auto begin() { return neurons_.begin(); }
    /// TODO: make iterator class.
    const auto end() const { return neurons_.cend(); }
    /// TODO: make iterator class.
    auto end() { return neurons_.end(); }

public:
    /**
     * @brief Neurons count.
     * @return neurons count in the population.
     */
    size_t size() const { return neurons_.size(); }

public:
    void operator()(const messaging::SynapticImpactMessage &synaptic_impact) {}

public:
    UID presynaptic_population_uid_;
    UID postsynaptic_population_uid_;
    std::vector<NeuronParameters> neurons_;
    BaseData base_;
};

}  // namespace knp::core
