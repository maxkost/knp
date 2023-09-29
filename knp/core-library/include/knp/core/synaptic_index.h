/**
 * @file synaptic_index.h
 * @brief Class used to index synapses inside a projection and facilitate faster access.
 * @author Vartenkov A.
 * @date 27.09.2023
 */

#pragma once

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>

/**
 * @brief Namespace for synaptic access and indexing auxiliary functions and classes.
 */
namespace knp::core::synapse_access
{
/**
 * @brief Connection description structure.
 */
struct Connection
{
    /**
     * @brief Presynaptic neuron index.
     */
    size_t from_;
    /**
     * @brief Postsynaptic neuron index.
     */
    size_t to_;

    /**
     * @brief Synapse index.
     */
    size_t index_;

    /**
     * @brief Comparison operator.
     * @param c another connection.
     * @return true if connections have the same parameters and direction.
     */
    bool operator==(const Connection &c) const { return from_ == c.from_ && to_ == c.to_ && index_ == c.index_; }
};


/**
 * @brief The Index class is a class used for fast search for synapses.
 */
class Index
{
public:
    /**
     * @brief Add connection to index.
     * @param connection connection to add.
     */
    void insert(const Connection &connection) { val_.insert(connection); }

    /**
     * @brief Clear index.
     */
    void clear() { val_.clear(); }

    /**
     * @brief Find a range of synapses connected to a presynaptic neuron.
     * @param presynaptic_neuron_index index of the presynaptic neuron.
     * @return a pair of iterators (begin, end) for the range that corresponds to the neuron.
     */
    auto find_by_presynaptic(size_t presynaptic_neuron_index)
    {
        return val_.get<mi_presynaptic>().equal_range(presynaptic_neuron_index);
    }

    /**
     * @brief Find a range of synapses connected to a postsynaptic neuron.
     * @param postsynaptic_neuron_index index of the postsynaptic neuron.
     * @return a pair of iterators (begin, end) for the range that corresponds to the neuron.
     */
    auto find_by_postsynaptic(size_t postsynaptic_neuron_index)
    {
        return val_.get<mi_postsynaptic>().equal_range(postsynaptic_neuron_index);
    }

    /**
     * @brief Remove a synapse with a given index.
     * @param index index of a synapse.
     * @return true if an element was found and erased.
     */
    bool erase(size_t index)
    {
        auto num_erased = val_.get<mi_synapse_index>().erase(index);
        return num_erased > 0;
    }

private:
    boost::multi_index::multi_index_container<
        Connection,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<struct mi_presynaptic>, BOOST_MULTI_INDEX_MEMBER(Connection, size_t, from_)>,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<struct mi_postsynaptic>, BOOST_MULTI_INDEX_MEMBER(Connection, size_t, to_)>,
            boost::multi_index::hashed_unique<
                boost::multi_index::tag<struct mi_synapse_index>,
                BOOST_MULTI_INDEX_MEMBER(Connection, size_t, index_)> > >
        val_;
};
}  // namespace knp::core::synapse_access
