/**
 * @file projection.cpp
 * @brief Projection class implementation.
 * @author Andrey V.
 * @date 26.01.2023
 */

#include <knp/core/projection.h>

#include <spdlog/spdlog.h>


// Index functions
template <class Index, class Connection>
void insert_to_index(Index &val, const Connection &connection)
{
    val.insert(connection);
}


template <class Index, class Type>
auto find_by_type(const Index &val, size_t neuron_index)
{
    return val.template get<Type>().equal_range(neuron_index);
}


template <class Index, class Type>
std::vector<size_t> find_indexes_by_type(const Index &val, size_t neuron_index)
{
    auto range = find_by_type<Index, Type>(val, neuron_index);
    std::vector<size_t> res;
    std::transform(range.first, range.second, std::back_inserter(res), [](const auto &val) { return val.index_; });
    return res;
}


/**
 * @brief Remove a synapse with a given index.
 * @param index index of a synapse.
 * @return true if an element was found and erased.
 */
template <class Index, class Projection>
bool erase_synapse(Index &val, size_t index)
{
    auto num_erased = val.template get<typename Projection::mi_synapse_index>().erase(index);
    return num_erased > 0;
}


/**
 * @brief Helper function, removes elements by their indexes in a single pass.
 * @tparam T value type.
 * @param data vector that will be modified by deletion.
 * @param to_remove indexes of the elements to be removed. MUST BE SORTED!
 */
template <class T, class IndexContainer>
void remove_by_index(std::vector<T> &data, const IndexContainer &to_remove)
{
    if (to_remove.empty()) return;
    size_t removed = 0;
    auto next_to_remove = to_remove.begin();
    size_t index = *next_to_remove;
    while (index + removed < data.size())
    {
        while (index + removed == *next_to_remove)
        {
            ++next_to_remove;
            ++removed;
        }
        data[index] = std::move(data[index + removed]);
        ++index;
    }
    data.resize(data.size() - removed);
}


namespace knp::core
{
using Connection = typename std::tuple<size_t, size_t, size_t>;


template <typename SynapseType>
Projection<SynapseType>::Projection(UID presynaptic_uid, UID postsynaptic_uid)  // !OCLint (Parameters are used)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}", std::string(get_uid()),
        std::string(presynaptic_uid_), std::string(postsynaptic_uid_));
}


template <typename SynapseType>
Projection<SynapseType>::Projection(UID uid, UID presynaptic_uid, UID postsynaptic_uid)  // !OCLint(Parameters are used)
    : base_{uid}, presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}", std::string(get_uid()),
        std::string(presynaptic_uid_), std::string(postsynaptic_uid_));
}


template <typename SynapseType>
Projection<SynapseType>::Projection(
    UID presynaptic_uid, UID postsynaptic_uid, SynapseGenerator generator, size_t num_iterations)  // !OCLint
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}, i = {}",
        std::string(get_uid()), std::string(presynaptic_uid_), std::string(postsynaptic_uid_), num_iterations);
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto params = generator(i))
        {
            parameters_.emplace_back(std::move(params.value()));
        }
    }
    reindex();
}


template <typename SynapseType>
Projection<SynapseType>::Projection(
    UID uid, UID presynaptic_uid, UID postsynaptic_uid, SynapseGenerator generator, size_t num_iterations)  // !OCLint
    : base_{uid}, presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}, i = {}",
        std::string(get_uid()), std::string(presynaptic_uid_), std::string(postsynaptic_uid_), num_iterations);
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto params = generator(i))
        {
            auto &&[p, id_from, id_to] = std::move(params.value());
            parameters_.emplace_back(Synapse{std::move(p), id_from, id_to});
        }
    }
    reindex();
}


template <typename SynapseType>
std::vector<size_t> knp::core::Projection<SynapseType>::find_synapses(size_t neuron_id, Search search_criterion) const
{
    reindex();
    std::vector<size_t> res;
    switch (search_criterion)
    {
        case Search::by_postsynaptic:
            res = find_indexes_by_type<decltype(index_), core::Projection<SynapseType>::ByPostsynaptic>(
                index_, neuron_id);
            break;
        case Search::by_presynaptic:
            res =
                find_indexes_by_type<decltype(index_), core::Projection<SynapseType>::ByPresynaptic>(index_, neuron_id);
            break;
        default:
            return {};
    }
    return res;
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::add_synapses(SynapseGenerator generator, size_t num_iterations)
{
    const size_t starting_size = parameters_.size();
    is_index_updated_ = false;
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto data = generator(i))
        {
            parameters_.emplace_back(std::move(data.value()));
        }
    }
    return parameters_.size() - starting_size;
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::add_synapses(const std::vector<Synapse> &synapses)
{
    const size_t starting_size = parameters_.size();
    bool was_index_updated_ = is_index_updated_;
    // Ensure that index is recalculated on exception.
    is_index_updated_ = false;
    for (const auto &synapse : synapses)
    {
        if (was_index_updated_)
            index_.insert(
                {std::get<knp::core::source_neuron_id>(synapse), std::get<knp::core::target_neuron_id>(synapse),
                 parameters_.size()});
        parameters_.push_back(synapse);
    }
    is_index_updated_ = was_index_updated_;
    return parameters_.size() - starting_size;
}


template <typename SynapseType>
void Projection<SynapseType>::clear()
{
    parameters_.clear();
    index_.clear();
}


template <typename SynapseType>
void knp::core::Projection<SynapseType>::remove_synapse(size_t index)
{
    is_index_updated_ = false;
    parameters_.erase(parameters_.begin() + index);
}


template <typename SynapseType>
void knp::core::Projection<SynapseType>::remove_synapses(const std::vector<size_t> &)
{
    throw std::runtime_error("Not implemented");
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_if(std::function<bool(const Synapse &)> predicate)
{
    const size_t starting_size = parameters_.size();
    is_index_updated_ = false;
    parameters_.resize(std::remove_if(parameters_.begin(), parameters_.end(), predicate) - parameters_.begin());
    return starting_size - parameters_.size();
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_postsynaptic_neuron(size_t neuron_index)
{
    const size_t starting_size = parameters_.size();
    bool was_index_updated = is_index_updated_;
    // Basic exception safety.
    is_index_updated_ = false;
    auto synapses_to_remove = find_synapses(neuron_index, Search::by_postsynaptic);
    std::sort(synapses_to_remove.begin(), synapses_to_remove.end());
    remove_by_index(parameters_, synapses_to_remove);
    if (was_index_updated)
        for (auto &synapse : synapses_to_remove) index_.erase(synapse);

    is_index_updated_ = was_index_updated;
    return starting_size - parameters_.size();
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_presynaptic_neuron(size_t neuron_index)
{
    // TODO: We now have a way to find them quickly, make use of it instead of disconnect_if.
    return disconnect_if([neuron_index](const Synapse &synapse)
                         { return std::get<knp::core::source_neuron_id>(synapse) == neuron_index; });
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_neurons(size_t neuron_from, size_t neuron_to)
{
    return disconnect_if(
        [neuron_from, neuron_to](const Synapse &synapse)
        {
            return (std::get<knp::core::source_neuron_id>(synapse) == neuron_from) &&
                   (std::get<knp::core::target_neuron_id>(synapse) == neuron_to);
        });
}


template <typename SynapseType>
void knp::core::Projection<SynapseType>::reindex() const
{
    if (is_index_updated_)
    {
        return;
    }

    index_.clear();
    for (size_t i = 0; i < parameters_.size(); ++i)
    {
        auto &synapse = parameters_[i];
        insert_to_index(
            index_,
            Connection{
                std::get<knp::core::source_neuron_id>(synapse), std::get<knp::core::target_neuron_id>(synapse), i});
    }
    is_index_updated_ = true;
}


#define INSTANCE_PROJECTIONS(n, template_for_instance, synapse_type) \
    template class knp::core::Projection<knp::synapse_traits::synapse_type>;

BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::core
