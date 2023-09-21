/**
 * @file projection.h
 * @brief General Projection Interface.
 * @author Artiom N.
 * @date 18.01.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/projection_support.h>
#include <knp/core/uid.h>
#include <knp/synapse-traits/all_traits.h>

#include <algorithm>
#include <functional>
#include <optional>
#include <tuple>
#include <unordered_map>
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
     * @brief Type of the projection synapses.
     */
    using ProjectionSynapseType = SynapseType;
    /**
     * @brief Projection of synapses with the specified synapse type.
     */
    using ProjectionType = Projection<SynapseType>;
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
        SynapseParameters params_;

        /**
         * @brief Index of a neuron from which the synapse receives spikes (presynaptic neuron).
         */
        uint32_t id_from_;

        /**
         * @brief Index of a neuron that the synapse influences (postsynaptic neuron).
         */
        uint32_t id_to_;
    };

    /**
     * @brief Synapse generation function.
     * @deprecated Must be removed.
     */
    using SynapseGenerator = std::function<std::optional<Synapse>(uint32_t)>;
    /**
     * @brief Synapse generation function type.
     */
    using SynapseGenerator1 = std::function<std::optional<std::tuple<SynapseParameters, uint32_t, uint32_t>>(uint32_t)>;

    /**
     * @brief Type of shared synapse parameters.
     */
    using SharedSynapseParameters = knp::synapse_traits::shared_synapse_parameters<SynapseType>;

public:
    /**
     * @brief Construct an empty projection.
     * @param presynaptic_uid UID of the presynaptic population.
     * @param postsynaptic_uid UID of the postsynaptic population.
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid);
    /**
     * @brief Construct an empty projection.
     * @param uid projection UID.
     * @param presynaptic_uid UID of the presynaptic population.
     * @param postsynaptic_uid UID of the postsynaptic population.
     */
    Projection(UID uid, UID presynaptic_uid, UID postsynaptic_uid);

    /**
     * @brief Construct a projection by running a synapse generator a given number of times.
     * @param presynaptic_uid presynaptic population UID.
     * @param postsynaptic_uid postsynaptic population UID.
     * @param generator function that generates a synapse.
     * @param num_iterations number of iterations to run the synapse generator.
     * @deprecated Must be removed.
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator &generator, size_t num_iterations);

    /**
     * @brief Construct a projection by running a synapse generator a given number of times.
     * @param presynaptic_uid presynaptic population UID.
     * @param postsynaptic_uid postsynaptic population UID.
     * @param generator function that generates synapse parameters: `parameteres`, `id_from_`, `id_to_`.
     * @param num_iterations number of times to run the synapse generator.
     */
    Projection(UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator1 &generator, size_t num_iterations);

    /**
     * @brief Construct a projection by running a synapse generator a given number of times.
     * @param uid projection UID.
     * @param presynaptic_uid presynaptic population UID.
     * @param postsynaptic_uid postsynaptic population UID.
     * @param generator function that generates synapse parameters: `parameteres`, `id_from_`, `id_to_`.
     * @param num_iterations number of times to run the synapse generator.
     */
    Projection(
        UID uid, UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator1 &generator, size_t num_iterations);

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
     * @param index synapse index.
     */
    [[nodiscard]] Synapse &operator[](size_t index) { return parameters_[index]; }
    /**
     * @brief Get parameter values of a synapse with the given index.
     * @details Constant method.
     * @param index synapse index.
     */
    [[nodiscard]] const Synapse &operator[](size_t index) const { return parameters_[index]; }

    /**
     * @brief Get an iterator pointing to the first element of the projection.
     * @return constant projection iterator.
     */
    [[nodiscard]] auto begin() const { return parameters_.cbegin(); }
    /**
     * @brief Get an iterator pointing to the first element of the projection.
     * @return projection iterator.
     */
    [[nodiscard]] auto begin() { return parameters_.begin(); }
    /**
     * @brief Get an iterator pointing to the last element of the projection.
     * @return constant iterator.
     */
    [[nodiscard]] auto end() const { return parameters_.cend(); }
    // TODO: This might be dangerous if you change index_from or index_to of a synapse, without updating.
    /**
     * @brief Get an iterator pointing to the last element of the projection.
     * @return iterator.
     */
    [[nodiscard]] auto end() { return parameters_.end(); }

public:
    /**
     * @brief Count number of synapses in the projection.
     * @return number of synapses.
     */
    [[nodiscard]] size_t size() const { return parameters_.size(); }

    /**
     * @brief Get UID of the associated population from which this projection receives spikes.
     * @return UID of the presynaptic population.
     */
    [[nodiscard]] const UID &get_presynaptic() const { return presynaptic_uid_; }

    /**
     * @brief Get UID of the associated population to which this projection sends signals.
     * @return UID of the postsynaptic population.
     */
    [[nodiscard]] const UID &get_postsynaptic() const { return postsynaptic_uid_; }

    /**
     * @brief Calculate connection parameters for a synapse with the given index.
     * @param index index of the projection synapse.
     * @return presynaptic neuron index, synapse index, postsynaptic neuron index.
     */
    [[nodiscard]] knp::core::Connection get_connection(size_t index) const
    {
        return {parameters_[index].id_from_, parameters_[index].id_to_, index};
    }

    /**
     * @brief Find synapses that originate from a neuron with the given index.
     * @param neuron_index index of a presynaptic neuron.
     * @return indexes of all synapses associated with the specified presynaptic neuron.
     */
    [[nodiscard]] std::vector<size_t> get_by_presynaptic_neuron(size_t neuron_index) const;

    /**
     * @brief Find synapses connected to the neuron with the given index.
     * @param neuron_index index of a postsynaptic neuron.
     * @return indexes of all synapses associated with the specified postsynaptic neuron.
     */
    [[nodiscard]] std::vector<size_t> get_by_postsynaptic_neuron(size_t neuron_index) const;

    /**
     * @brief Calculate connection parameters for all synapses in the projection.
     */
    [[nodiscard]] std::vector<Connection> get_connections() const;

    /**
     * @brief Append connections to the existing projection.
     * @param num_iterations number of iterations to run the synapse generator.
     * @param generator synapse generation function.
     * @return number of synapses added to the projection, which can be less or equal to the `num_iterations` value.
     */
    size_t add_synapses(size_t num_iterations, const SynapseGenerator &generator);

    /**
     * @brief Add a set of user-defined synapses to the projection.
     * @param synapses vector of synapses to add to the projection.
     * @note The method might create duplicate synapses.
     * @return number of synapses added to the projection.
     */
    size_t add_synapses(const std::vector<Synapse> &synapses);

    /**
     * @brief Remove all synapses from the projection.
     */
    void clear()
    {
        parameters_.clear();
        index_.clear();
    }

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
     * @tparam Predicate functor that determines if the synapse must be deleted (derived automatically from `predicate`
     * if not specified).
     * @param predicate functor that receives a synapse and returns `true` if the synapse must be deleted.
     * @return number of deleted synapses.
     */
    template <class Predicate>
    size_t disconnect_if(Predicate predicate)
    {
        const size_t starting_size = parameters_.size();
        is_index_updated_ = false;
        parameters_.resize(std::remove_if(parameters_.begin(), parameters_.end(), predicate) - parameters_.begin());
        return starting_size - parameters_.size();
    }

    /**
     * @brief Remove all synapses that lead to a neuron with the given index.
     * @param neuron_index index of the postsynaptic neuron which related synapses must be deleted.
     * @return number of deleted synapses.
     */
    size_t disconnect_postsynaptic_neuron(size_t neuron_index);

    /**
     * @brief Remove all synapses that receive signals from a neuron with the given index.
     * @param neuron_index index of the presynaptic neuron which related synapses must be deleted.
     * @return number of deleted synapses.
     */
    size_t disconnect_presynaptic_neuron(size_t neuron_index);

    /**
     * @brief Remove all synapses between two neurons with given indexes.
     * @param neuron_from index of the presynaptic neuron.
     * @param neuron_to index of the postsynaptic neuron.
     * @return number of deleted synapses.
     */
    size_t disconnect_neurons(size_t neuron_from, size_t neuron_to);

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
     * @return `true` if the synapse weight change is locked, `false` if the synapse weight change is not locked.
     */
    bool is_locked() { return is_locked_; }

public:
    /**
     * @brief Get parameters shared between all synapses.
     * @return shared parameters.
     */
    SharedSynapseParameters &get_shared_parameters() { return shared_parameters_; }
    /**
     * @brief Get parameters shared between all synapses.
     * @note Constant method.
     * @return shared parameters.
     */
    const SharedSynapseParameters &get_shared_parameters() const { return shared_parameters_; }

private:
    void reindex() const;

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
     * @brief Return `false` if the weight change for synapses is not locked.
     */
    bool is_locked_ = false;

    /**
     * @brief Container of synapse parameters.
     */
    std::vector<Synapse> parameters_;
    mutable Index index_;  // So far index is mutable so we can reindex a const object that has an non-updated index.
    mutable bool is_index_updated_ = false;

    SharedSynapseParameters shared_parameters_;
};


/**
 * @brief List of projection types based on synapse types specified in `knp::synapse_traits::AllSynapses`.
 * @details `AllProjections` takes the value of `Projection<SynapseType_1>, Projection<SynapseType_2>, ...,
 * Projection<SynapseType_n>`, where `SynapseType_[1..n]` is the synapse type specified in
 * `knp::synapse_traits::AllSynapses`. \n For example, if `knp::synapse_traits::AllSynapses` contains DeltaSynapse and
 * AdditiveSTDPSynapse types, then `AllProjections` = `Population<DeltaSynapse>, Population<AdditiveSTDPSynapse>`.
 */
using AllProjections = boost::mp11::mp_transform<knp::core::Projection, knp::synapse_traits::AllSynapses>;

/**
 * @brief Projection variant that contains any projection type specified in `AllProjections`.
 * @details `AllProjectionVariants` takes the value of `std::variant<ProjectionType_1,..., ProjectionType_n>`, where
 * `ProjectionType_[1..n]` is the projection type specified in `AllProjections`. \n For example, if `AllProjections`
 * contains DeltaSynapse and AdditiveSTDPSynapse types, then `AllProjectionVariants = std::variant<DeltaSynapse,
 * AdditiveSTDPSynapse>`. \n `AllProjectionVariants` retains the same order of message types as defined in
 * `AllProjections`.
 * @see ALL_SYNAPSES.
 */
using AllProjectionsVariant = boost::mp11::mp_rename<AllProjections, std::variant>;

}  // namespace knp::core
