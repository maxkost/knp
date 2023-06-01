/**
 * @brief Input interpreters header.
 * @author Vartenkov Andrey
 * @date 31.06.2023
 */
#pragma once


namespace knp::framework::input
{
/**
 * @brief A very simple function to turn values to spike or not. Casts a value to boolean type.
 * @tparam value_type input value type. Something that can be cast to bool implicitly. Usually an arithmetic type.
 * @param v input value
 * @return bool(v)
 */
template <class value_type>
bool interpret_as_bool(value_type v)
{
    return v;
}


/**
 * @brief Construct a function to turn values to spikes.
 * @tparam value_type input value type, should have a defined operator "less than".
 * @param threshold threshold value, spikes when greater or equal than threshold.
 * @return a simple function to determine if a value causes a spike or not.
 */
template <class value_type>
auto interpret_with_threshold(value_type threshold)
{
    // We're using !(a < b) instead of (a >= b) because of the values which only have operator "less than".
    return [threshold](value_type v) -> bool { return !(v < threshold); };
}

}  // namespace knp::framework::input
