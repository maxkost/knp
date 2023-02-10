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
#include <vector>

namespace knp::core
{
/**
 * @brief The Projection class is a definition of similar connections between the neurons of two populations.
 * @note I think this class should later be divided to interface and implementation classes.
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
     * Construct an empty projection
     * @param presynaptic_uid the Uid of the presynaptic population
     * @param postsynaptic_uid the Uid of the postsynaptic population
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid)
            : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid) {}

    /**
     * Construct projection by running a synapse generator N times
     * @param num_iterations number of iterations
     * @param generator a function that returs
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid, size_t num_iterations, SynapseGenerator &generator)
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
     * Get this projection UID.
     * @return UID.
     */

    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    /**
     * Get this projection tags.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * Calculate synapse parameters based on its index and its ItemClass
     * @param synapse_index index of the synapse
     * @return a container of synapse parameters
     */
    [[nodiscard]] SynapseParameters &get_synapse_parameters(size_t synapse_index) const
    {
        return parameters_[synapse_index].params;
    }

    /**
     * Calculate (if needed) and return all the synaptic parameters for the projection
     * @return vector of parameters and connections, one structure for each synapse
     */
    [[nodiscard]] const auto &get_synapses_parameters() const { return parameters_; }

    /**
     * Get the number of synapses
     * @return number of synapses inside the projection
     */
    [[nodiscard]] size_t size() { return parameters_.size(); }

    /**
     * Set new synapse parameter
     * @param new_parameters new synapse parameters.
     * @param synapse_index index of the synapse.
     */
    void set_synapse_parameter(SynapseParameters &&new_parameters, size_t index)
    {
        parameters_[index] == new_parameters;
    }

    /**
     * Return the UID of population this projection gets signals from
     * @return presynaptic population UID
     */
    [[nodiscard]] UID get_presynaptic() const { return presynaptic_uid_; }

    /**
     * Return the UID of population this projection sends signals to
     * @return postsynaptic population UID
     */
    [[nodiscard]] UID get_postsynaptic() const { return postsynaptic_uid_; }

    /**
     * Get synaptic parameters and connection for a given index
     * @param index the index of the synapse
     * @return synapse parameters and connection
     */
    [[nodiscard]] Synapse& operator[](size_t index) { return parameters_[index]; }
    [[nodiscard]] const Synapse& operator[](size_t index) const {return parameters_[index]; }

    /**
     * Calculate connection parameters for a given synapse index
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
    size_t add_synapses(size_t num_iterations, SynapseGenerator &generator)
    {
        size_t starting_size = parameters_.size();
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
     * Add a set of user-made synapses to the projection
     * @param synapses the set of synapses that will be added to the container
     * @note may create duplicates
     * @return number of synapses that were added (should be the same as synapses.size())
     */
    size_t add_synapses(std::vector<Synapse> synapses)
    {
        size_t starting_size = parameters_.size();
        std::move(synapses.begin(), synapses.end(), std::back_insert_iterator(parameters_));
        return parameters_.size() - starting_size;
    }

    /**
     * Remove a synapse by its index
     * @param index index of the synapse to be removed
     */
    void remove_synapse(size_t index) { parameters_.erase(parameters_.begin() + index); }

    /**
     * Remove synapses according to a given criterion
     * @param predicate a functor that receives SynapseValue and returns true if the synapse must be deleted
     * @return the number of deleted synapses
     */
    template<class Predicate>
    size_t disconnect_if(Predicate predicate)
    {
        const size_t starting_size = parameters_.size();
        parameters_.resize(std::remove_if(parameters_.begin(), parameters_.end(), predicate) - parameters_.begin());
        return starting_size - parameters_.size();
    }

    /**
     * Remove all the connections that lead to the neuron with given id
     * @param neuron_index the index of postsynaptic neuron that will be deleted
     * @return number of removed connections
     */
    size_t remove_postsynaptic_neuron(size_t neuron_index)
    {
        return disconnect_if([neuron_index](const Synapse &synapse) {return synapse.id_to == neuron_index;});
    }

    /**
     * Remove all the connections that lead from the neuron with given id
     * @param neuron_index the index of presynaptic neuron that will be deleted
     * @return number of removed connections
     */
    size_t remove_presynaptic_neuron(size_t neuron_index)
    {
        return disconnect_if([neuron_index](const Synapse &synapse) {return synapse.id_from == neuron_index;});
    }

    /**
     * Remove any connections between two neurons
     * @param neuron_from
     * @param neuron_to
     * @return number of removed connections
     */
    size_t disconnect_neurons(size_t neuron_from, size_t neuron_to)
    {
        return disconnect_if(
                [neuron_from, neuron_to](const Synapse &synapse)
                {
                    return (synapse.id_from == neuron_from) && (synapse.id_to == neuron_to);
                }
        );
    }


public:
    /**
     * Lock synapses weights.
     */
    void lock_weights() { is_locked_ = true; }

    /**
     * Unlock synapses weights.
     */
    void unlock_weights() { is_locked_ = false; }

    /**
     * Return true if the projection is locked
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
