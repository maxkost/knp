/**
 * @file projection.h
 * @brief General Projection Interface.
 * @author Artiom N.
 * @date 18.01.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/uid.h>
#include <knp/synapse-traits/output_types.h>
#include <knp/synapse-traits/type_traits.h>

#include <algorithm>
#include <functional>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

/**
 * @brief Core library namespace.
 */
namespace knp::core
{
/**
 * @brief The Projection class is a definition of similar connections between the neurons of two populations.
 * @note This class should later be divided to interface and implementation classes.
 * @tparam SynapseType type of synapses the projection contains.
 * @see ALL_SYNAPSES.
 */
template <class SynapseType>
class Projection
{
public:
    /**
     * @brief Parameters of the specified synapse type.
     */
    using SynapseParameters = typename synapse_traits::synapse_parameters<SynapseType>;

    /**
     * @brief Synapse description structure that contains synapse parameters and indexes of the associated neurons.
     */
    struct Synapse
    {
        /**
         * @brief Synapse parameters. For example, parameters can contain synapse weight and delay.
         */
        SynapseParameters params;

        /**
         * @brief Index of a neuron from which the synapse receives spikes (presynaptic neuron).
         */
        uint32_t id_from;

        /**
         * @brief Index of a neuron that the synapse influences (postsynaptic neuron).
         */
        uint32_t id_to;
    };

    /**
     * @brief Synapse generation function.
     */
    using SynapseGenerator = std::function<std::optional<Synapse>(uint32_t)>;

    /**
     * @brief Construct an empty projection.
     * @param presynaptic_uid UID of the presynaptic population.
     * @param postsynaptic_uid UID of the postsynaptic population.
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid)
        : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
    {
    }

    /**
     * @brief Construct a projection by running a synapse generator a given number of times.
     * @param presynaptic_uid presynaptic population UID.
     * @param postsynaptic_uid postsynaptic population UID.
     * @param generator function that generates a synapse.
     * @param num_iterations number of iterations to run the synapse generator.
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator &generator, size_t num_iterations)
        : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
    {
        for (size_t i = 0; i < num_iterations; ++i)
        {
            if (auto params = std::move(generator(i)))
            {
                parameters_.emplace_back(std::move(params.value()));
            }
        }
    }

public:
    /**
     * @brief Get projection UID.
     * @return projection UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * @brief Get tags used by the projection.
     * @return projection tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Get parameter values of a synapse with the given index.
     */
    [[nodiscard]] Synapse &operator[](size_t index) { return parameters_[index]; }
    /**
     * @brief Get parameter values of a synapse with the given index.
     * @details Constant method.
     */
    [[nodiscard]] const Synapse &operator[](size_t index) const { return parameters_[index]; }

    // TODO: add custom iterator class
    [[nodiscard]] auto begin() const { return parameters_.cbegin(); }
    // TODO: add custom iterator class
    [[nodiscard]] auto begin() { return parameters_.begin(); }
    // TODO: add custom iterator class
    [[nodiscard]] auto end() const { return parameters_.cend(); }
    // TODO: add custom iterator class
    [[nodiscard]] auto end() { return parameters_.end(); }

public:
    /**
     * @brief Count number of synapses in the projection.
     * @return synapse count.
     */
    [[nodiscard]] size_t size() { return parameters_.size(); }

    /**
     * @brief Get UID of the associated population from which this projection receives spikes.
     * @return UID of the presynaptic population.
     */
    [[nodiscard]] UID get_presynaptic() const { return presynaptic_uid_; }

    /**
     * @brief Get UID of the associated population to which this projection sends signals.
     * @return UID of the postsynaptic population.
     */
    [[nodiscard]] UID get_postsynaptic() const { return postsynaptic_uid_; }

    /**
     * @brief Calculate connection parameters for a synapse with the given index.
     * @param index index of the projection synapse.
     * @return presynaptic neuron index, synapse index, postsynaptic neuron index.
     */
    [[nodiscard]] std::tuple<size_t, size_t, size_t> get_connection(size_t index) const
    {
        return std::make_tuple(parameters_[index].id_from, index, parameters_[index].id_to);
    }

    // TODO: VERY inefficient. Will need to optimize it to less than linear ASAP
    /**
     * @brief Find synapses that originate from a neuron with the given index.
     * @param neuron_index index of a presynaptic neuron.
     * @return indexes of all synapses associated with the specified presynaptic neuron.
     */
    [[nodiscard]] std::vector<size_t> get_by_presynaptic_neuron(size_t neuron_index) const
    {
        std::vector<size_t> res;
        for (size_t i = 0; i < parameters_.size(); ++i)
        {
            if (parameters_[i].id_from == neuron_index)
            {
                res.push_back(i);
            }
        }
        return res;
    }

    /**
     * @brief Calculate connection parameters for all synapses in the projection.
     */
    [[nodiscard]] std::vector<std::tuple<size_t, size_t, size_t>> get_connections() const
    {
        std::vector<std::tuple<size_t, size_t, size_t>> result;
        result.reserve(parameters_.size());
        for (size_t i = 0; i < parameters_.size(); ++i)
        {
            result.emplace_back(std::make_tuple(parameters_[i].id_from, i, parameters_[i].id_to));
        }
        return result;
    }

    /**
     * @brief Append connections to the existing projection.
     * @param num_iterations number of iterations to run the synapse generator.
     * @param generator synapse generation function.
     * @return number of synapses added to the projection, which can be less or equal to the `num_iterations` value.
     */
    size_t add_synapses(size_t num_iterations, const SynapseGenerator &generator)
    {
        const size_t starting_size = parameters_.size();
        for (size_t i = 0; i < num_iterations; ++i)
        {
            if (auto data = std::move(generator(i)))
            {
                parameters_.emplace_back(std::move(data.value()));
            }
        }
        return parameters_.size() - starting_size;
    }

    /**
     * @brief Add a set of user-defined synapses to the projection.
     * @param synapses vector of synapses to add to the projection.
     * @note The method might create duplicate synapses.
     * @return number of synapses added to the projection.
     */
    size_t add_synapses(const std::vector<Synapse> &synapses)
    {
        const size_t starting_size = parameters_.size();
        std::copy(synapses.begin(), synapses.end(), std::back_insert_iterator(parameters_));
        return parameters_.size() - starting_size;
    }

    /**
     * @brief Remove all synapses from the projection.
     */
    void clear() { parameters_.clear(); }

    /**
     * @brief Remove a synapse with the given index from the projection.
     * @param index index of the synapse to remove.
     */
    void remove_synapse(size_t index) { parameters_.erase(parameters_.begin() + index); }

    /**
     * @brief Remove synapses with the given indexes from the projection.
     * @param indexes indexes of synapses to remove.
     */
    void remove_synapses(const std::vector<size_t> &indexes) {}  // TODO: implement this.

    /**
     * @brief Remove synapses according to a given criterion.
     * @tparam Predicate functor that determines if the synapse must be deleted (derived automatically from `predicate` if not specified).
     * @param predicate functor that receives a synapse and returns true if the synapse must be deleted.
     * @return number of deleted synapses.
     */
    template <class Predicate>
    size_t disconnect_if(Predicate predicate)
    {
        const size_t starting_size = parameters_.size();
        parameters_.resize(std::remove_if(parameters_.begin(), parameters_.end(), predicate) - parameters_.begin());
        return starting_size - parameters_.size();
    }

    /**
     * @brief Remove all synapses that lead to a neuron with the given index.
     * @param neuron_index index of the postsynaptic neuron which related synapses must be deleted.
     * @return number of deleted synapses.
     */
    size_t disconnect_postsynaptic_neuron(size_t neuron_index)
    {
        return disconnect_if([neuron_index](const Synapse &synapse) { return synapse.id_to == neuron_index; });
    }

    /**
     * @brief Remove all synapses that receive signals from a neuron with the given index.
     * @param neuron_index index of the presynaptic neuron which related synapses must be deleted.
     * @return number of deleted synapses.
     */
    size_t disconnect_presynaptic_neuron(size_t neuron_index)
    {
        return disconnect_if([neuron_index](const Synapse &synapse) { return synapse.id_from == neuron_index; });
    }

    /**
     * @brief Remove all synapses between two neurons with given indexes.
     * @param neuron_from index of the presynaptic neuron.
     * @param neuron_to index of the postsynaptic neuron.
     * @return number of deleted synapses.
     */
    size_t disconnect_neurons(size_t neuron_from, size_t neuron_to)
    {
        return disconnect_if([neuron_from, neuron_to](const Synapse &synapse)
                             { return (synapse.id_from == neuron_from) && (synapse.id_to == neuron_to); });
    }

public:
    /**
     * @brief Lock the possibility to change synapses weights.
     */
    void lock_weights() { is_locked_ = true; }

    /**
     * @brief Unlock the possibility to change synapses weights.
     */
    void unlock_weights() { is_locked_ = false; }

    /**
     * @brief Determine if the synapse weight change is locked.
     * @return true if the synapse weight change is locked, false if the synapse weight change is not locked.
     */
    bool is_locked() { return is_locked_; }

private:
    BaseData base_;

    /**
     * @brief UID of the population that sends spikes to the projection (presynaptic population)
     */
    UID presynaptic_uid_;

    /**
     * @brief UID of the population that receives synapse responses from this projection (postsynaptic population).
     */
    UID postsynaptic_uid_;

    /**
     * @brief Return false if the weight change for synapses is not locked.
     */
    bool is_locked_ = false;

    // TODO Change this container into something that searches efficiently by input index. A multiindex or
    // unordered_multimap
    /**
     * @brief Container of synapse parameters.
     */
    std::vector<Synapse> parameters_;
};

}  // namespace knp::core
