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
#include <knp/neuron-traits/all_traits.h>

#include <functional>
#include <utility>
#include <vector>

/**
 * @brief Core library namespace.
 */
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
     * @brief Type of the population neurons.
     */
    using PopulationNeuronType = NeuronType;
    /**
     * @brief Population of neurons with the specified neuron type.
     */
    using PopulationType = Population<NeuronType>;
    /**
     * @brief Neuron parameters and their values for the specified neuron type.
     */
    using NeuronParameters = neuron_traits::neuron_parameters<NeuronType>;

    /**
     * @brief Type of the neuron generator.
     */
    using NeuronGenerator = std::function<NeuronParameters(size_t index)>;

public:
    /**
     * @brief Construct a population by running a neuron generator.
     * @param generator neuron generator.
     * @param neurons_count number of time to run the neuron generator.
     */
    Population(NeuronGenerator generator, size_t neurons_count) { add_neurons(generator, neurons_count); }

public:
    /**
     * @brief Get population UID.
     * @return population UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * @brief Get tags used by the population.
     * @return population tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Get parameters of all neurons in the population.
     * @return vector of neuron parameters.
     */
    [[nodiscard]] const std::vector<NeuronParameters> &get_neurons_parameters() const { return neurons_; }

    /**
     * @brief Get parameters of the specific neuron in the population.
     * @param index index of the population neuron.
     * @return vector of neuron parameters.
     */
    [[nodiscard]] const NeuronParameters &get_neuron_parameters(size_t index) const { return neurons_[index]; }

    /**
     * @brief Set parameters for the specific neuron in the population.
     * @param index index of the population neuron.
     * @param parameters vector of neuron parameters defined in NeuronParameters for the population.
     */
    void set_neuron_parameters(size_t index, NeuronParameters &&parameters) { neurons_[index] = std::move(parameters); }

    /**
     * @brief Set parameters for the specific neuron in the population.
     * @param index index of the population neuron.
     * @param parameters vector of neuron parameters defined in NeuronParameters for the population.
     */
    void set_neurons_parameters(size_t index, const NeuronParameters &parameters) { neurons_[index] = parameters; }

public:
    /**
     * @brief Get tags used by neuron with the specified index.
     * @param index index of the population neuron.
     * @return tag map of the neuron.
     */
    [[nodiscard]] const TagMap &get_neuron_tags(size_t index)
    {
        return base_.tags_.template get_tag<std::vector<TagMap>>("neuron_tags")[index];
    }

public:
    /**
     * @brief Add neurons to the population.
     * @param generator type of the neuron generator.
     * @param count number of times to run the neuron generator.
     */
    void add_neurons(NeuronGenerator generator, size_t count)
    {
        neurons_.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            neurons_.template emplace_back(std::move(generator(i)));
        }
    }

    /**
     * @brief Remove neurons with given indexes from the population.
     * @param neuron_indexes indexes of neurons to remove.
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
     * @brief Remove a specific neuron from the population.
     * @param neuron_index index of the neuron to remove.
     */
    void remove_neuron(const size_t &neuron_index)
    {
        auto iter = neurons_.begin();
        std::advance(iter, neuron_index);
        neurons_.erase(iter);
    }

public:
    /**
     * @brief Get parameter values of a neuron with the given index.
     * @note Constant method.
     * @param index neuron index.
     */
    const auto &operator[](const size_t index) const { return get_neuron_parameters(index); }
    /**
     * @brief Get parameter values of a neuron with the given index.
     * @param index neuron index.
     */
    auto &operator[](const size_t index) { return neurons_[index]; }
    /// TODO: make iterator class.
    auto begin() const { return neurons_.cbegin(); }
    /// TODO: make iterator class.
    auto begin() { return neurons_.begin(); }
    /// TODO: make iterator class.
    auto end() const { return neurons_.cend(); }
    /// TODO: make iterator class.
    auto end() { return neurons_.end(); }

public:
    /**
     * @brief Count number of neurons in the population.
     * @return neuron count.
     */
    size_t size() const { return neurons_.size(); }

private:
    std::vector<NeuronParameters> neurons_;
    BaseData base_;
};


/**
 * @brief List of population types based on neuron types specified in `knp::neuron_traits::AllNeurons`.
 * @details `AllPopulations` takes the value of `Population<NeuronType_1>, Population<NeuronType_2>, ...,
 * Population<NeuronType_n>`, where `NeuronType_[1..n]` is the neuron type specified in
 * `knp::neuron_traits::AllNeurons`. \n For example, if `knp::neuron_traits::AllNeurons` contains BLIFATNeuron and
 * IzhikevichNeuron types, then `AllPopulations` = `Population<BLIFATNeuron>, Population<IzhikevichNeuron>`.
 */
using AllPopulations = boost::mp11::mp_transform<knp::core::Population, knp::neuron_traits::AllNeurons>;

/**
 * @brief Population variant that contains any population type specified in `AllPopulations`.
 * @details `AllPopulationVariants` takes the value of `std::variant<PopulationType_1,..., PopulationType_n>`, where
 * `PopulationType_[1..n]` is the population type specified in `AllPopulations`. \n For example, if `AllPopulations`
 * contains BLIFATNeuron and IzhikevichNeuron types, then `AllPopulationVariants = std::variant<BLIFATNeuron,
 * IzhikevichNeuron>`. \n `AllPopulationVariants` retains the same order of message types as defined in
 * `AllPopulations`.
 * @see ALL_NEURONS.
 */
using AllPopulationsVariant = boost::mp11::mp_rename<AllPopulations, std::variant>;

}  // namespace knp::core
