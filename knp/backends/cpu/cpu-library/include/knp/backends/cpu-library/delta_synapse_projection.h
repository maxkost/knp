/**
 * @file delta_synapse_projection.h
 * @brief DeltaSynapse calculation routines definition.
 * @author Artiom N.
 * @date 21.02.2023
 */

#pragma once

#include <knp/core/message_bus.h>
#include <knp/core/projection.h>
#include <knp/synapse-traits/delta.h>

#include <mutex>
#include <unordered_map>

/**
 * @brief Type of the message queue.
 */
typedef std::unordered_map<uint64_t, knp::core::messaging::SynapticImpactMessage> MessageQueue;


/**
 * @brief Process a part of a spike message for a projection.
 * @param projection projection that receives the message.
 * @param message_in spike message for the projection.
 * @param future_messages future messages queue.
 * @param step_n current step.
 * @param part_start index of the starting spike.
 * @param part_size number of spikes to process.
 * @param mutex mutex.
 */
void calculate_projection_part(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection,
    const knp::core::messaging::SpikeMessage &message_in, MessageQueue &future_messages, u_int64_t step_n,
    size_t part_start, size_t part_size, std::mutex &mutex);


/**
 * @brief Make one execution step for a projection of Delta synapses.
 * @param projection projection to update.
 * @param endpoint message endpoint used for message exchange.
 * @param future_messages message queue to process via endpoint.
 * @param step_n execution step.
 */
void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n);

/**
 * @brief Make one execution step for a projection of Delta synapses.
 * @param projection projection to update.
 * @param endpoint message endpoint used for message exchange.
 * @param future_messages message queue to process via endpoint.
 * @param step_n execution step.
 * @param m mutex.
 */
void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint,
    MessageQueue &future_messages, size_t step_n, std::mutex &m);
