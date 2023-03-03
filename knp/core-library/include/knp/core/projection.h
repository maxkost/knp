/**
 * @file projection.h
 * @brief General Projection Interface.
 * @author Artiom N.
 * @date 18.01.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/uid.h>
#include <knp/synapse-traits/type_traits.h>

#include <functional>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>


namespace knp::core
{
/**
 * @brief The Projection class is a definition of similar connections between the neurons of two populations.
 * @note This class should later be divided to interface and implementation classes.
 */
template <class ItemClass>
class Projection
{
public:
    using SynapseParameters = typename synapse_traits::synapse_parameters<ItemClass>;
    struct Synapse
    {
        SynapseParameters params;
        size_t id_from;
        size_t id_to;
    };
    using SynapseGenerator = std::function<std::optional<Synapse>(size_t)>;

    /**
     * @brief Construct an empty projection
     * @param presynaptic_uid the Uid of the presynaptic population
     * @param postsynaptic_uid the Uid of the postsynaptic population
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid)
        : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
    {
    }

    /**
     * @brief Construct projection by running a synapse generator N times
     * @param num_iterations number of iterations
     * @param generator a function that returns
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid, size_t num_iterations, SynapseGenerator generator)
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

    Projection(UID presynaptic_uid, UID postsynaptic_uid, size_t num_iterations, const SynapseGenerator &generator)
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
     * @brief Get this projection UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }

    /**
     * @brief Get this projection tags.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /// Synapse indexing operator
    [[nodiscard]] Synapse &operator[](size_t index) { return parameters_[index]; }
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
     * @brief Get the number of synapses
     * @return number of synapses inside the projection
     */
    [[nodiscard]] size_t size() { return parameters_.size(); }

    /**
     * @brief Return the UID of population this projection gets signals from
     * @return presynaptic population UID
     */
    [[nodiscard]] UID get_presynaptic() const { return presynaptic_uid_; }

    /**
     * @brief Return the UID of population this projection sends signals to
     * @return postsynaptic population UID
     */
    [[nodiscard]] UID get_postsynaptic() const { return postsynaptic_uid_; }

    /**
     * @brief Calculate connection parameters for a given synapse index
     * @param index synapse index
     * @return presynaptic neuron index, synapse index, postsynaptic neuron index
     */
    [[nodiscard]] std::tuple<size_t, size_t, size_t> get_connection(size_t index) const
    {
        return std::make_tuple(parameters_[index].id_from, index, parameters_[index].id_to);
    }

    /**
     * @brief calculate connections parameters from data
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
     * @brief Add connections to the existing array
     * @param num_iterations number of iterations to run the generator
     * @param generator a functional object that is used to generate connections
     * @return number of added connections, which can be less or equal to num_iterations
     */
    size_t add_synapses(size_t num_iterations, SynapseGenerator generator)
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
     * @brief Add a set of user-made synapses to the projection
     * @param synapses the set of synapses that will be added to the container
     * @note may create duplicates
     * @return number of synapses that were added (should be the same as synapses.size())
     */
    size_t add_synapses(std::vector<Synapse> synapses)
    {
        const size_t starting_size = parameters_.size();
        std::move(synapses.begin(), synapses.end(), std::back_insert_iterator(parameters_));
        return parameters_.size() - starting_size;
    }

    /// Remove all synapses
    void clear() { parameters_.clear(); }

    /**
     * @brief Remove a synapse by its index
     * @param index index of the synapse to be removed
     */
    void remove_synapse(size_t index) { parameters_.erase(parameters_.begin() + index); }

    /**
     * @brief Remove synapses according to a given criterion
     * @param predicate a functor that receives SynapseValue and returns true if the synapse must be deleted
     * @return the number of deleted synapses
     */
    template <class Predicate>
    size_t disconnect_if(Predicate predicate)
    {
        const size_t starting_size = parameters_.size();
        parameters_.resize(std::remove_if(parameters_.begin(), parameters_.end(), predicate) - parameters_.begin());
        return starting_size - parameters_.size();
    }

    /**
     * @brief Remove all the connections that lead to the neuron with given id
     * @param neuron_index the index of postsynaptic neuron that will be deleted
     * @return number of removed connections
     */
    size_t remove_postsynaptic_neuron(size_t neuron_index)
    {
        return disconnect_if([neuron_index](const Synapse &synapse) { return synapse.id_to == neuron_index; });
    }

    /**
     * @brief Remove all the connections that lead from the neuron with given id
     * @param neuron_index the index of presynaptic neuron that will be deleted
     * @return number of removed connections
     */
    size_t remove_presynaptic_neuron(size_t neuron_index)
    {
        return disconnect_if([neuron_index](const Synapse &synapse) { return synapse.id_from == neuron_index; });
    }

    /**
     * @brief Remove any connections between two neurons
     * @param neuron_from
     * @param neuron_to
     * @return number of removed connections
     */
    size_t disconnect_neurons(size_t neuron_from, size_t neuron_to)
    {
        return disconnect_if([neuron_from, neuron_to](const Synapse &synapse)
                             { return (synapse.id_from == neuron_from) && (synapse.id_to == neuron_to); });
    }

public:
    /**
     * @brief Lock synapses weights.
     */
    void lock_weights() { is_locked_ = true; }

    /**
     * @brief Unlock synapses weights.
     */
    void unlock_weights() { is_locked_ = false; }

    /**
     * @brief Return true if the projection is locked
     */
    bool is_locked() { return is_locked_; }

private:
    BaseData base_;

    /// Uid of the population that sends spikes to this projection (presynaptic)
    UID presynaptic_uid_;

    /// Uid of the population that receives synapse responses from this projection (postsynaptic)
    UID postsynaptic_uid_;

    /// If the weights are locked for internal modification (unused).
    bool is_locked_ = false;

    /// Parameters container.
    std::vector<Synapse> parameters_;
};

}  // namespace knp::core
