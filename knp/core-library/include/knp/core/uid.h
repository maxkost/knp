/**
 * @file uid.h
 * @brief UID class and routines.
 * @author Artiom N.
 * @date 01.02.2021
 */

#pragma once

#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <utility>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/**
 * @brief Core library namespace.
 */
namespace knp::core
{

#if defined(_DEBUG) || defined(DEBUG)
#    define _ENABLE_PSEUDO_UID_GENERATOR 1
#endif

/**
 * @brief UID generator for entities.
 * @details You can use this class for debugging. UID is displayed as a 128-bit number.
 */
class continuously_uid_generator
{
public:
    ::boost::uuids::uuid operator()() const;
    /**
     * @brief Reset UID counter.
     */
    void reset(uint64_t initial_value = 1);
};

#if defined(_ENABLE_PSEUDO_UID_GENERATOR)
#    define uid_generator continuously_uid_generator
#else
#    define uid_generator ::boost::uuids::random_generator
#endif


/**
 * @brief The UID class is a definition of unique identifiers for entities.
 */
struct UID
{
    // TODO: Optimize it.
    /**
     * @brief Generate a new UID.
     * @param random if true generates a random UID, else return zero UID (nil)
     */
    explicit UID(bool random = true) : tag(random ? uid_generator()() : ::boost::uuids::nil_uuid()) {}

    /**
     * @brief Make a UID from boost::uuids::uuid.
     * @param guid value to be copied to UID.
     */
    explicit UID(const ::boost::uuids::uuid &guid) : tag(guid) {}
    explicit UID(::boost::uuids::uuid &&guid) : tag(std::move(guid)) {}

    /**
     * @brief Make a UID from an array of 16 uint_8t integers.
     * @param guid_value values that will be stored as a UID.
     */
    explicit UID(const std::array<
                 boost::uuids::uuid::value_type, sizeof(boost::uuids::uuid::data) / sizeof(boost::uuids::uuid::data[0])>
                     &guid_value)
    {
        std::copy(guid_value.begin(), guid_value.end(), tag.data);
    }

    /**
     * @brief Copy constructor;
     */
    UID(const UID &) = default;

    /**
     * @brief Convert UID to boost::uuids::uuid.
     * @return stored UID value.
     */
    operator const ::boost::uuids::uuid &() const { return tag; }
    operator ::boost::uuids::uuid &() { return tag; }

    /**
     * @brief Convert UID to string.
     * @return string representation of the UID.
     */
    explicit operator ::std::string() const
    {
        std::stringstream ss;
        ss << tag;
        return ss.str();
    }

    /**
     * @brief Check if UID is valid (nonzero).
     * @return true if UID is valid.
     */
    explicit operator bool() const { return !tag.is_nil(); }

    /**
     * @brief Comparison operator for sorting.
     * @param uid the other UID.
     * @return true if this UID is less than the other.
     */
    bool operator<(const UID &uid) const { return uid.tag < tag; }

    /**
     * @brief Check if two UIDs are equal.
     * @param uid the other UID.
     * @return true if both UIDs are equal.
     */
    bool operator==(const UID &uid) const { return uid.tag == tag; }

    /**
     * @brief Check if two UIDs are different.
     * @param uid the other UID.
     * @return true if the UIDs are different.
     */
    bool operator!=(const UID &uid) const { return uid.tag != tag; }

    /**
     * @brief UID value.
     */
    ::boost::uuids::uuid tag;
};


/**
 * @brief Send UID to a stream.
 * @param s stream.
 * @param uid UID.
 * @return modified stream.
 */
inline ::std::ostream &operator<<(std::ostream &s, const UID &uid)
{
    s << uid.tag;
    return s;
}

/**
 * @brief get UID from a stream.
 * @param s stream.
 * @param uid UID.
 * @return modified stream.
 */
inline ::std::istream &operator>>(std::istream &s, UID &uid)
{
    s >> uid.tag;
    return s;
}

}  // namespace knp::core
