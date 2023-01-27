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
#include <tuple>
#include <vector>

namespace knp::core
{

/**
 * @brief The Projection class is definition of the similar connections between neurons of two populations.
 * @note I think this class should later be divided to interface and implementation classes.
 */
template <class ItemClass>
class Projection
{
public:
    using SynapseParameters = typename synapse_traits::synapse_parameters<ItemClass>::SynapseParameters;
    using SynapseGenerator = std::function<ItemClass(size_t)>;

    Projection(size_t starting_size, SynapseGenerator generator)
    {
        parameters_.reserve(starting_size);
        for (size_t i = 0; i < starting_size; ++i)
        {
            parameters_.push_back(generator(i));
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
     * Calculates synapse parameters based on its index and its ItemClass
     * @param synapse_index index of the synapse
     * @return a container of synapse parameters
     */
    [[nodiscard]] SynapseParameters &get_synapse_parameters(size_t synapse_index) const
    {
        return parameters_[synapse_index];
    }

    /**
     * Calculates (if needed) and returns all the synaptic parameters for the projection
     * @return vector of parameters, one structure for each synapse
     */
    [[nodiscard]] const std::vector<SynapseParameters> &get_synapses_parameters() const { return parameters_; }

    /**
     * Gets the number of synapses
     * @return number of synapses inside the projection
     */
    [[nodiscard]] size_t get_size() { return parameters_.size(); }

    /**
     * Sets new synapse parameter
     * @param new_parameters new synapse parameters.
     * @param synapse_index index of the synapse.
     */
    void set_synapse_parameter(SynapseParameters &&new_parameters, size_t index)
    {
        parameters_[index] == new_parameters;
    }

    /**
     * Returns the UID of population this projection gets signals from
     * @return presynaptic population UID
     */
    [[nodiscard]] UID get_presynaptic() const { return presynaptic_uid_; }

    /**
     * Returns the UID of population this projection sends signals to
     * @return postsynaptic population UID
     */
    [[nodiscard]] UID get_postsynaptic() const { return postsynaptic_uid_; }

    /**
     * Calculates connection parameters for a given synapse index
     * @param index synapse index
     * @return presynaptic neuron index, synapse index, postsynaptic neuron index
     */
    [[nodiscard]] std::tuple<size_t, size_t, size_t> &get_connection(size_t index) const
    {
        return std::make_tuple(parameters_[index].pre_id, index, parameters_[index].post_id);
    }

    /**
     * @brief calculates connections parameters from data
     */
    [[nodiscard]] const std::vector<std::tuple<size_t, size_t, size_t>> &get_connections() const;

public:
    /**
     * Reset synapses weights.
     */
    void reset_weights();

    /**
     * Lock synapses weights.
     */
    void lock_weights();

    /**
     * Unlock synapses weights.
     */
    void unlock_weights();

private:
    BaseData base_;
    UID presynaptic_uid_;
    UID postsynaptic_uid_;

    /// Parameters container.
    std::vector<SynapseParameters> parameters_;
};

}  // namespace knp::core
