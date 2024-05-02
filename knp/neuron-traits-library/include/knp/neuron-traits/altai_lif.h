/**
 * @file altai_lif.h
 * @brief AltAILIF neuron type traits.
 * @author Alexey Lubiakin
 * @date 02.05.2024
 */

#pragma once

#include <cstdint>
#include <limits>
#include <numeric>
#include <utility>

#include "type_traits.h"


/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{

/**
 * @brief AltAILIF neuron. Use as a template parameter only.
 */
struct AltAILIF;


/**
 * @brief Structure for AltAILIF neuron default values.
 */
template <>
struct default_values<AltAILIF>
{
    /**
     * @brief The parameter defines the default value for 'is_diff_' flag of AltAILIF neuron.
     * This flag defines after spike behaviour of the AltAILIF neuron.
     */
    constexpr static bool is_diff_ = false;

    /**
     * @brief The parameter defines the default value for 'is_reset_' flag of AltAILIF neuron.
     * This flag defines after spike behaviour of the AltAILIF neuron.
     */
    constexpr static bool is_reset_ = true;

    /**
     * @brief The parameter defines the default value for 'leak_rev_' flag of AltAILIF neuron.
     * This flag defines whether the leak sign will be changed based on the current sign of the 'potential_'.
     */
    constexpr static bool leak_rev_ = true;

    /**
     * @brief The parameter defines the default value for 'saturate_' flag of AltAILIF neuron.
     * This flag defines after spike behaviour related to 'negative_activation_threshold_'.
     */
    constexpr static bool saturate_ = true;

    /**
     * @brief The parameter defines the default value for 'do_not_save_' flag of AltAILIF neuron.
     * This flag defines whether 'potential_' will be stored with each time stamp.
     */
    constexpr static bool do_not_save_ = false;

    /**
     * @brief The parameter defines the initial value for 'potential_' state of AltAILIF neuron.
     * @details Additional packet will be send to AltAI-2 for each neuron, with non-zero initial 'potential_' value.
     */
    constexpr static int16_t potential_ = 0;

    /**
     * @brief The parameter defines the default value for 'activation_threshold_' of AltAILIF neuron.
     */
    constexpr static uint16_t activation_threshold_ = 1;

    /**
     * @brief The parameter defines the default value for 'negative_activation_threshold_' of AltAILIF neuron.
     * @details The default value was chosen for the scenario where negative spikes aren't used and
     * 'negative_activation_threshold_'  with ('saturate_'=true) is used to protect agains 'potential_' negative
     * overflow.
     */
    constexpr static uint16_t negative_activation_threshold_ = 30000;

    /**
     * @brief The parameter defines the default value for 'potential_leak_' of AltAILIF neuron.
     */
    constexpr static int16_t potential_leak_ = 0;

    /**
     * @brief The parameter defines the default value for 'potential_reset_value_' of AltAILIF neuron.
     */
    constexpr static uint16_t potential_reset_value_ = 0;
};


/**
 * @brief Structure for AltAILIF neuron parameters.
 */
template <>
struct neuron_parameters<AltAILIF>
{
    /**
     * @brief This flag defines after spike behaviour of the AltAILIF neuron.
     * @details The code below demonstrate the logic of after spike flags for AltAILIF neuron:
     * @code{.cpp}
     * if (potential_ >= activation_threshold_)
     * {
     *  if (is_diff_) potential_ -= activation_threshold_;
     *  if (is_reset_) potential_ = potential_reset_value_;
     * }
     * else if (potential_ < -negative_activation_threshold_)
     * {
     *  if (saturate_) potential_ = negative_activation_threshold_;
     *  else {
     *      if (is_reset_) potential_ = -potential_reset_value_;
     *      else if (is_diff_) potential_ -= negative_activation_threshold_;
     *  }
     * }
     * @endcode
     */
    bool is_diff_ = default_values<AltAILIF>::is_diff_;

    /**
     * @brief This flag defines after spike behaviour of the AltAILIF neuron.
     * @details The code below demonstrate the logic of after spike flags for AltAILIF neuron:
     * @code{.cpp}
     * if (potential_ >= activation_threshold_)
     * {
     *  if (is_diff_) potential_ -= activation_threshold_;
     *  if (is_reset_) potential_ = potential_reset_value_;
     * }
     * else if (potential_ < -negative_activation_threshold_)
     * {
     *  if (saturate_) potential_ = negative_activation_threshold_;
     *  else {
     *      if (is_reset_) potential_ = -potential_reset_value_;
     *      else if (is_diff_) potential_ -= negative_activation_threshold_;
     *  }
     * }
     * @endcode
     */
    bool is_reset_ = default_values<AltAILIF>::is_reset_;

    /**
     * @brief This flag defines whether the leak sign will be changed based on the current sign of the 'potential_'.
     * @details The code below demonstrates the logic of 'leak_rev' flag:
     * @code{.cpp}
     * if (leak_rev_)
     *  potential_ += (potential_ < 0 ? -potential_leak_ : potential_leak_);
     * else
     *  potential_ += potential_leak_;
     * @endcode
     */
    bool leak_rev_ = default_values<AltAILIF>::leak_rev_;

    /**
     * @brief This flag defines after spike behaviour of the AltAILIF neuron.
     * @details The code below demonstrate the logic of after spike flags for AltAILIF neuron:
     * @code{.cpp}
     * if (potential_ >= activation_threshold_)
     * {
     *  if (is_diff_) potential_ -= activation_threshold_;
     *  if (is_reset_) potential_ = potential_reset_value_;
     * }
     * else if (potential_ < -negative_activation_threshold_)
     * {
     *  if (saturate_) potential_ = negative_activation_threshold_;
     *  else {
     *      if (is_reset_) potential_ = -potential_reset_value_;
     *      else if (is_diff_) potential_ -= negative_activation_threshold_;
     *  }
     * }
     * @endcode
     */
    bool saturate_ = default_values<AltAILIF>::saturate_;

    /**
     * @brief This flag defines whether 'potential_' will be stored with each time stamp.
     * @details If 'do_not_save_' is true, The potential will be equal to the 'potential_reset_value_'
     *  at the beginning of each subsequent time step (except first time step, when 'potential_'
     *  will be equal to 'potential_' default_value).
     */
    bool do_not_save_ = default_values<AltAILIF>::do_not_save_;

    /**
     * @brief State of the AltAILIF neuron.
     */
    int16_t potential_ = default_values<AltAILIF>::potential_;

    /**
     * @brief Defines the threshold value of the 'potential_', after exceeding which a positive spike can be emitted.
     * @details Positive spike is emitted if 'potential_' >= 'activation_threshold_'
     * and the neuron has a target for positive spike.
     */
    uint16_t activation_threshold_ = default_values<AltAILIF>::activation_threshold_;

    /**
     * @brief Defines the threshold value of the 'potential_', below which a negative spike can be emitted.
     * @details negative spike is emitted if 'potential_' < -'negative_activation_threshold_'
     * and the neuron has a target for negative spike.
     */
    uint16_t negative_activation_threshold_ = default_values<AltAILIF>::negative_activation_threshold_;

    /**
     * @brief Defines the constant leakage of the 'potential_' value.
     * @details The code below demonstrates how leakage mechanism works for AltAILIF neuron:
     * @code{.cpp}
     * if (leak_rev_)
     *  potential_ += (potential_ < 0 ? -potential_leak_ : potential_leak_);
     * else
     *  potential_ += potential_leak_;
     * @endcode
     */
    int16_t potential_leak_ = default_values<AltAILIF>::potential_leak_;

    /**
     * @brief Defines the reset_value of the 'potential_' after one of the thresholds has been exceeded.
     * @details The code below demonstrates in what case 'potential_reset_value_'
     * is used depending on all AltAILIF flags:
     * @code{.cpp}
     * if (potential_ >= activation_threshold_)
     * {
     *  if (is_diff_) potential_ -= activation_threshold_;
     *  if (is_reset_) potential_ = potential_reset_value_;
     * }
     * else if (potential_ < -negative_activation_threshold_)
     * {
     *  if (saturate_) potential_ = negative_activation_threshold_;
     *  else {
     *      if (is_reset_) potential_ = -potential_reset_value_;
     *      else if (is_diff_) potential_ -= negative_activation_threshold_;
     *  }
     * }
     * @endcode
     */
    uint16_t potential_reset_value_ = default_values<AltAILIF>::potential_reset_value_;
};

}  // namespace knp::neuron_traits
