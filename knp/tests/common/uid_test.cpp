/**
 * UUID tests.
 */

#include <knp/core/uid.h>

#include "tests_common.h"


TEST(UidSuite, UidGenerator)
{
    using uuid = ::boost::uuids::uuid;

    auto u = ::knp::core::continuously_uid_generator()();
    ASSERT_EQ(uuid{{1}}, u);
    ASSERT_NE(uuid{{2}}, u);

    auto u_gen = ::knp::core::continuously_uid_generator();

    for (uint8_t i = 2; i < 100; ++i) ASSERT_EQ(uuid{{i}}, u_gen());
}


TEST(UidSuite, UidToString)
{
    ::knp::core::UID uid{::boost::uuids::uuid{{1, 2, 3}}};

    ASSERT_EQ("01020300-0000-0000-0000-000000000000", std::string(uid));

    std::cout << uid << std::endl;
}
