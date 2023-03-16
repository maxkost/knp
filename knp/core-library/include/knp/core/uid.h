/**
 * @file uid.h
 * @brief UID class and routines.
 * @author Artiom N.
 * @date 01.02.2021
 */

#pragma once

#include <sstream>
#include <string>
#include <utility>

#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


namespace knp::core
{

#if defined(_DEBUG) || defined(DEBUG)
#    define _ENABLE_PSEUDO_UID_GENERATOR 1
#endif

/**
 * @brief Debug UID generator.
 *
 * UIDs will be like 0001-..., 0002-..., 0003-....
 */
class continuously_uid_generator
{
public:
    ::boost::uuids::uuid operator()() const;
};

#if defined(_ENABLE_PSEUDO_UID_GENERATOR)
#    define uid_generator continuously_uid_generator
#else
#    define uid_generator ::boost::uuids::random_generator
#endif


/**
 * @brief The UID class is an unique identifier type.
 */

struct UID
{
    // TODO: Optimize it.
    explicit UID(bool random = true) : tag(random ? uid_generator()() : ::boost::uuids::nil_uuid()) {}
    explicit UID(const ::boost::uuids::uuid &guid) : tag(guid) {}
    explicit UID(::boost::uuids::uuid &&guid) : tag(std::move(guid)) {}
    UID(const UID &) = default;

    explicit operator const ::boost::uuids::uuid &() const { return tag; }
    explicit operator ::std::string() const
    {
        std::stringstream ss;

        ss << tag;

        return ss.str();
    }

    explicit operator bool() const { return !tag.is_nil(); }

    bool operator<(const UID &uid) const { return uid.tag < tag; }
    bool operator==(const UID &uid) const { return uid.tag == tag; }
    bool operator!=(const UID &uid) const { return uid.tag != tag; }

    ::boost::uuids::uuid tag;
};


inline ::std::ostream &operator<<(std::ostream &s, const UID &uid)
{
    s << uid.tag;
    return s;
}

struct UID_hash
{
    auto operator()(UID value) { return boost::hash<boost::uuids::uuid>()(boost::uuids::uuid(value)); }
};

}  // namespace knp::core
