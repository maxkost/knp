/**
 * @brief Input interpreters header.
 * @author Vartenkov Andrey
 * @date 31.06.2023
 */
#pragma once


/**
 * @brief Input channel namespace.
 */
namespace knp::framework::input
{
/**
 * @brief Determine if an input value is a spike and convert an input value to boolean.
 * @tparam value_type input value type (usually an arithmetic type) that can be cast to bool implicitly.
 * @param v input value.
 * @return `true` if an input value is a spike, `false` if an input value is not a spike.
 */
template <class ValueType>
bool interpret_as_bool(ValueType v)
{
    return v;
}


/**
 * @brief Interpret an input value as a spike if an input value is equal or greater than a threshold value.
 * @tparam value_type input value type that should have a defined operator "less than".
 * @param threshold threshold value.
 * @return boolean function that determines if a value causes a spike or not.
 */
template <class ValueType>
auto interpret_with_threshold(ValueType threshold)
{
    // We're using !(a < b) instead of (a >= b) because of the values which only have operator "less than".
    return [threshold](ValueType v) -> bool { return !(v < threshold); };
}

}  // namespace knp::framework::input
