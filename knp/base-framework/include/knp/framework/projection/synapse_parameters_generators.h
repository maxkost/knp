/**
 * @file synapse_parameters_generators.h
 * @brief Parameters generators.
 * @kaspersky_support Artiom N.
 * @date 17.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/projection.h>

#include <functional>
#include <optional>

/**
 * @brief Projection namespace.
 */
namespace knp::framework::projection
{

/**
 * @brief Namespace for framework projection connectors.
 */
namespace parameters_generators
{

/**
 * @brief Two parameters (index, index) generator type.
 * @tparam SynapseType synapse type which parameters generated.
 */
template <typename SynapseType>
using SynGen2ParamsType = std::function<typename knp::core::Projection<SynapseType>::SynapseParameters(size_t, size_t)>;


/**
 * @brief One parameter (index) generator type.
 * @tparam SynapseType synapse type which parameters generated.
 */
template <typename SynapseType>
using SynGen1ParamType = std::function<typename knp::core::Projection<SynapseType>::SynapseParameters(size_t)>;


/**
 * @brief Two parameters (index, index) generator type with optional result.
 * @tparam SynapseType synapse type which parameters generated.
 */
template <typename SynapseType>
using SynGenOptional2ParamsType =
    std::function<typename std::optional<typename knp::core::Projection<SynapseType>::SynapseParameters>(
        size_t, size_t)>;


/**
 * @brief Default generator of synapse parameters.
 * @tparam SynapseType synapse type.
 * @return synapse parameters.
 */
template <typename SynapseType>
typename knp::core::Projection<SynapseType>::SynapseParameters default_synapse_gen(size_t, size_t)  // NOLINT
{
    return typename knp::core::Projection<SynapseType>::SynapseParameters();
}


/**
 * @brief Copy parameters generator of synapse parameters.
 * @tparam SynapseType synapse type.
 */
template <typename SynapseType>
class CopySynapseGen
{
public:
    /**
     * @brief Type of the synapse parameters.
     */
    using SynapseParametersType = typename knp::core::Projection<SynapseType>::SynapseParameters;

    /**
     * Copy synapse constructor.
     * @param base_synapse synapse parameters to copy.
     */
    explicit CopySynapseGen(const SynapseParametersType& base_synapse) : base_synapse_(base_synapse) {}

    /**
     * @brief Generation operator.
     * @return synapse parameters equal to base synapse parameters.
     */
    SynapseParametersType operator()(size_t, size_t) const  // NOLINT
    {
        return base_synapse_;
    }

private:
    SynapseParametersType base_synapse_;
};

}  // namespace parameters_generators

}  // namespace knp::framework::projection
