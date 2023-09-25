//
// Created by an_vartenkov on 06.09.23.
//

#pragma once
#include <vector>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>

/**
 * @brief Core library namespace.
 */
namespace knp::core
{
/**
 * @brief Connection description structure.
 */
struct Connection
{
    /**
     * @brief Index of a presynaptic neuron.
     */
    size_t from_;
    /**
     * @brief Index of a postsynaptic neuron.
     */
    size_t to_;

    /**
     * @brief Index of synapse parameters.
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
 * @brief A container of Connections, with fast search by any of its fields.
 */
typedef boost::multi_index::multi_index_container<
    Connection,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<struct mi_presynaptic>, BOOST_MULTI_INDEX_MEMBER(Connection, size_t, from_)>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<struct mi_postsynaptic>, BOOST_MULTI_INDEX_MEMBER(Connection, size_t, to_)>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<struct mi_synapse_index>, BOOST_MULTI_INDEX_MEMBER(Connection, size_t, index_)> > >
    Index;
}  // namespace knp::core
