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


void calculate_delta_synapse_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, knp::core::MessageEndpoint &endpoint);
