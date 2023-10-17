/**
 * @file delta_synapse_projection_impl.h
 * @brief DeltaSynapse and type index calculation routines implementation header.
 * @author Artiom N.
 * @date 21.08.2023
 */

#pragma once

#include <knp/backends/cpu-library/delta_synapse_projection.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <utility>
#include <vector>


namespace knp::backends::cpu
{

using knp::core::UID;
using knp::core::messaging::SpikeMessage;


}  // namespace knp::backends::cpu
