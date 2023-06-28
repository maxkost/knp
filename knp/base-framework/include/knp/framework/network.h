/**
 * @file network.h
 * @brief Network interface.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/impexp.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/coordinates/generator.h>
#include <knp/neuron-traits/all_traits.h>
#include <knp/synapse-traits/all_traits.h>

#include <variant>
#include <vector>


/**
 * @brief Framework namespace.
 */
namespace knp::framework
{

/**
 * @brief The Network class is a definition of a neural network that contains populations and projections.
 */
class Network
{
public:
    /**
     * @brief List of population types based on neuron types specified in `knp::neuron_traits::AllNeurons`.
     * @details `AllPopulations` takes the value of `Population<NeuronType_1>, Population<NeuronType_2>, ...,
     * Population<NeuronType_n>`, where `NeuronType_[1..n]` is the neuron type specified in
     * `knp::neuron_traits::AllNeurons`. \n For example, if `knp::neuron_traits::AllNeurons` containes BLIFATNeuron and
     * IzhikevichNeuron types, then `AllPopulations` = `Population<BLIFATNeuron>, Population<IzhikevichNeuron>`.
     */
    using AllPopulations = boost::mp11::mp_transform<knp::core::Population, knp::neuron_traits::AllNeurons>;

public:
    /**
     * @brief Type of population container.
     */
    using PopulationContainer = std::vector<core::AllPopulationsVariant>;
    /**
     * @brief Type of projection container.
     */
    using ProjectionContainer = std::vector<core::AllProjectionsVariant>;

    /**
     * @brief Types of population iterators.
     */
    using PopulationIterator = PopulationContainer::iterator;
    /**
     * @brief Types of constant population iterators.
     */
    using PopulationConstIterator = PopulationContainer::const_iterator;

    /**
     * @brief Types of projection iterators.
     */
    using ProjectionIterator = ProjectionContainer::iterator;
    /**
     * @brief Types of constant projection iterators.
     */
    using ProjectionConstIterator = ProjectionContainer::const_iterator;

public:
    /**
     * @brief Default network constructor.
     */
    Network() = default;

public:
    /**
     * @brief Add a population to the network.
     * @param population population to add.
     */
    void add_population(core::AllPopulationsVariant &&population);
    /**
     * @brief Add a population to the network.
     * @tparam PopulationType type of population to add (derived automatically from `population` if not specified).
     * @param population population to add.
     */
    template <typename PopulationType>
    void add_population(PopulationType &&population);
    /**
     * @brief Get a population with the given UID from the network.
     * @tparam PopulationType type of population to get.
     * @param population_uid population UID.
     * @throw `std::logic_error` if population is not found in the network.
     * @return population.
     */
    template <typename PopulationType>
    [[nodiscard]] PopulationType &get_population(const knp::core::UID &population_uid);
    /**
     * @brief Get a population with the given UID from the network.
     * @note Constant method.
     * @tparam PopulationType type of population to get.
     * @param population_uid population UID.
     * @throw `std::logic_error` if population is not found in the network.
     * @return population.
     */
    template <typename PopulationType>
    [[nodiscard]] const PopulationType &get_population(const knp::core::UID &population_uid) const;
    /**
     * @brief Remove a population with the given UID from the network.
     * @param population_uid UID of the population to remove.
     */
    void remove_population(const knp::core::UID &population_uid);

public:
    /**
     * @brief Add a projection to the network.
     * @param projection projection to add.
     */
    void add_projection(core::AllProjectionsVariant &&projection);
    /**
     * @brief Add a projection to the network.
     * @tparam ProjectionType type of projection to add (derived automatically from `projection` if not specified).
     * @param projection projection to add.
     */
    template <typename ProjectionType>
    void add_projection(ProjectionType &&projection);
    /**
     * @brief Get a projection with the given UID from the network.
     * @tparam ProjectionType type of projection to get.
     * @param projection_uid projection UID.
     * @throw `std::logic_error` if projection is not found in the network.
     * @return projection.
     */
    template <typename ProjectionType>
    [[nodiscard]] ProjectionType &get_projection(const knp::core::UID &projection_uid);
    /**
     * @brief Get a projection with the given UID from the network.
     * @note Constant method.
     * @tparam ProjectionType type of projection to get.
     * @param projection_uid projection UID.
     * @throw `std::logic_error` if projection is not found in the network.
     * @return projection.
     */
    template <typename ProjectionType>
    [[nodiscard]] const ProjectionType &get_projection(const knp::core::UID &projection_uid) const;
    /**
     * @brief Remove a projection with the given UID from the network.
     * @param projection_uid UID of the projection to remove.
     */
    void remove_projection(const knp::core::UID &projection_uid);

public:
    [[nodiscard]] PopulationIterator begin_populations();
    [[nodiscard]] PopulationConstIterator begin_populations() const;
    [[nodiscard]] PopulationIterator end_populations();
    [[nodiscard]] PopulationConstIterator end_populations() const;

    [[nodiscard]] ProjectionIterator begin_projections();
    [[nodiscard]] ProjectionConstIterator begin_projections() const;
    [[nodiscard]] ProjectionIterator end_projections();
    [[nodiscard]] ProjectionConstIterator end_projections() const;

public:
    [[nodiscard]] size_t populations_count() const { return populations_.size(); }
    [[nodiscard]] size_t projections_count() const { return projections_.size(); }

public:
    /**
     * @brief Get the network UID.
     * @return UID.
     */
    [[nodiscard]] const knp::core::UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get tags used by the network.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

private:
    template <typename T, typename VT>
    typename std::vector<VT>::iterator find_elem(const knp::core::UID &uid, std::vector<VT> &container);
    template <typename Ts, typename VT>
    typename std::vector<VT>::iterator find_variant(const knp::core::UID &uid, std::vector<VT> &container);

private:
    knp::core::BaseData base_;
    PopulationContainer populations_;
    ProjectionContainer projections_;
};

}  // namespace knp::framework
