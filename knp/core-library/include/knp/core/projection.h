/**
 * @file projection.h
 * @brief General Projection Interface.
 * @author Artiom N.
 * @date 18.01.2023
 */


#pragma once

#include <vector>
#include <functional>
#include <tuple>
#include "knp/core/uid.h"
#include "knp/core/core.h"
#include "knp/synapse-traits/type_traits.h"

namespace knp::core
{

/**
 * @brief The Projection class is definition of the similar connections between neurons of two populations.
 * @note I think this class should later be divided to interface and implementation classes.
 */

template<class ItemClass>
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


    /**
     * Calculates synapse parameters based on its index and its ItemClass
     * @param n index of the synapse
     * @return a container of synapse parameters
     */
    SynapseParameters get_synapse_parameters(size_t n) { return parameters_[n]; }


    /**
     * Calculates (if needed) and returns all the synaptic parameters for the projection
     * @return vector of parameters, one structure for each synapse
     */
    const std::vector<SynapseParameters>& get_synapses_parameters() { return parameters_; }


    /**
     * Gets the number of synapses
     * @return number of synapses inside the projection
     */
    size_t get_size() { return parameters_.size(); }


    /**
     * Sets new synapse parameter
     * @param new_parameters
     * @param index
     */
    void set_synapse_parameter(SynapseParameters new_parameters, size_t index)
    {
        parameters_[index] == std::move(new_parameters);
    }

    /**
     * A function to return tags associated with population
     * @return tag container
     */
    TagMap get_tags() { return base_.tags_; }

    /**
     * Returns the UID of population this projection gets signals from
     * @return presynaptic population UID
     */
    UID get_presynaptic() { return presynaptic_uid_; }

    /**
    * Returns the UID of population this projection sends signals to
    * @return postsynaptic population UID
    */
    UID get_postsynaptic() { return postsynaptic_uid_; }

    /**
     * Calculates connection parameters for a given synapse index
     * @param index synapse index
     * @return presynaptic neuron index, synapse index, postsynaptic neuron index
     */
    std::tuple<size_t, size_t, size_t> get_connection(size_t index)
    {
        return std::make_tuple(parameters_[index].pre_id, index, parameters_[index].post_id);
    }

    /**
     * @brief calculates connections parameters from data
     */
    std::vector<std::tuple<size_t, size_t, size_t>> get_connections();


private:
    BaseData base_;
    UID presynaptic_uid_;
    UID postsynaptic_uid_;

    // Parameters container
    std::vector<SynapseParameters> parameters_;
};

}  // namespace knp::core
