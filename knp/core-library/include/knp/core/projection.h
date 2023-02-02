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
 * @brief The Projection class is definition of the similar connections between neurons of two populations.
 * @note I think this class should later be divided to interface and implementation classes.
 */
template <class ItemClass>
class Projection
{
public:
    using SynapseConnection = std::pair<size_t, size_t>;
    using SynapseParameters = typename synapse_traits::synapse_parameters<ItemClass>::SynapseParameters;
    using SynapseGenerationType = std::pair<SynapseParameters, SynapseConnection>;
    using SynapseGenerator = std::function<std::optional<SynapseGenerationType>(size_t)>;

    Projection(size_t num_iterations, SynapseGenerator generator) : is_locked_(false)
    {
        for (size_t i = 0; i < num_iterations; ++i)
        {
            auto params = generator(i);
            if (params)
            {
                parameters_.push_back(std::move(params));
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
     * Calculates synapse parameters based on its index and its ItemClass
     * @param synapse_index index of the synapse
     * @return a container of synapse parameters
     */
    [[nodiscard]] SynapseParameters &get_synapse_parameters(size_t synapse_index) const
    {
        return parameters_[synapse_index].first;
    }

    /**
     * Calculates (if needed) and returns all the synaptic parameters for the projection
     * @return vector of parameters and connections, one structure for each synapse
     */
    [[nodiscard]] const auto &get_synapses_parameters() const { return parameters_; }

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
    [[nodiscard]] std::vector<std::tuple<size_t, size_t, size_t>> get_connections() const
    {
        std::vector<std::tuple<size_t, size_t, size_t>> result(parameters_.size());
        for (size_t i = 0; i < parameters_.size(); ++i)
        {
            result[i] = std::make_tuple(parameters_[i].second.first, i, parameters_[i].second.second);
        }
        return result;

    }

    /**
     * @brief adds connections to the existing array
     * @param num_iterations number of iterations to run the generator
     * @param generator a functional object that is used to generate connections
     * @return number of added connections, which can be less or equal to num_iterations
     */
    [[nodiscard]] size_t add_synapses(size_t num_iterations, SynapseGenerator generator)
    {
        size_t counter = 0;
        for (size_t i = 0; i < num_iterations; ++i)
        {
            auto data = generator(i);
            if (data)
            {
                parameters_.push_back(std::move(data));
                ++counter;
            }
        }
        return counter;
    }

    /**
     * Removes all the connections that lead to the neuron with given id
     * @param neuron_index the index of postsynaptic neuron that will be deleted
     * @return number of removed connections
     */
    size_t remove_postsynaptic_neuron(size_t neuron_index)
    {
        size_t starting_size = parameters_.size();
        std::remove_if(parameters_.begin(), parameters_.end(),
                       [&](std::pair<SynapseParameters, SynapseConnection> &param){return param.second == neuron_index;});
        return starting_size - parameters_.size();
    }

    /**
     * Removes all the connections that lead from the neuron with given id
     * @param neuron_index the index of presynaptic neuron that will be deleted
     * @return number of removed connections
     */
    size_t remove_presynaptic_neuron(size_t neuron_index)
    {
        size_t starting_size = parameters_.size();
        std::remove_if(parameters_.begin(), parameters_.end(),
                       [&](std::pair<SynapseParameters, SynapseConnection> &param){return param.first == neuron_index;});
        return starting_size - parameters_.size();
    }
    

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
    bool is_locked_;

    /// Parameters container.
    std::vector<std::pair<SynapseParameters, SynapseConnection>> parameters_;
};

}  // namespace knp::core
