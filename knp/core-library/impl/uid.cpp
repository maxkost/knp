/**
 * @brief UID implementation.
 * @author Artiom N.
 * @date 22.05.20
 */

#include <knp/core/uid.h>

#include <atomic>
#include <iostream>


namespace knp::core
{

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static ::std::atomic_uint64_t tag_counter = 1;


::boost::uuids::uuid continuously_uid_generator::operator()() const
{
    ::boost::uuids::uuid result = {{0}};
    const auto c_value{tag_counter++};
    constexpr auto value_size = sizeof(c_value);

    static_assert(
        sizeof(decltype(tag_counter)::value_type) <= ::boost::uuids::uuid::static_size(), "Uid counter size error!");

    for (size_t i = 0; i < value_size; ++i)
    {
        *(result.begin() + i) = static_cast<uint8_t>((c_value >> 8 * (value_size - i)) & 0xff);
    }

    return result;
}

}  // namespace knp::core
