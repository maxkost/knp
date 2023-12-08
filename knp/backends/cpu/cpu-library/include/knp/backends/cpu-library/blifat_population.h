/**
 * @file blifat_population.h
 * @brief Common templates for Blifat population.
 * @author A. Vartenkov
 * @date 07.11.2023
 */
#pragma once
#include <knp/backends/cpu-library/impl/blifat_population_impl.h>

/**
 * @brief Namespace for CPU backends.
 */
namespace knp::backends::cpu
{
/**
 * @brief Make one execution step for a population of BLIFAT neurons.
 * @param population population to update.
 * @param endpoint message endpoint used for message exchange.
 * @param step_n execution step.
 * @return indexes of spiked neurons.
 */
template <class BlifatLikeNeuron>
std::optional<core::messaging::SpikeMessage> calculate_blifat_population(
    knp::core::Population<BlifatLikeNeuron> &population, knp::core::MessageEndpoint &endpoint, size_t step_n)
{
    return calculate_blifat_population_impl(population, endpoint, step_n);
}

/**
 * @brief Make one execution step for a population of BLIFAT neurons.
 * @param population population to update.
 * @param endpoint message endpoint used for message exchange.
 * @param step_n execution step.
 * @param mutex mutex.
 * @return indexes of spiked neurons.
 */
template <class BlifatLikeNeuron>
std::optional<knp::core::messaging::SpikeMessage> calculate_blifat_population(
    knp::core::Population<BlifatLikeNeuron> &population, knp::core::MessageEndpoint &endpoint, size_t step_n,
    std::mutex &mutex)
{
    return calculate_blifat_population_impl(population, endpoint, step_n, mutex);
}

}  // namespace knp::backends::cpu
