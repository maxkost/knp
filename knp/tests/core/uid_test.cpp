/**
 * UUID tests.
 */

#include <knp/core/uid.h>

#include "tests_common.h"


TEST(UidSuite, UidGenerator)
{
    using uuid = ::boost::uuids::uuid;

    auto gen = ::knp::core::continuously_uid_generator();
    gen.reset();
    auto u = gen();

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


TEST(UidSuite, UidEq)
{
    ::knp::core::UID uid1{::boost::uuids::uuid{{1, 2, 3}}};
    ::knp::core::UID uid2{::boost::uuids::uuid{{1, 2, 3}}};
    ::knp::core::UID uid3{::boost::uuids::uuid{{3, 2, 1}}};
    ::knp::core::UID uid4(uid1);

    ASSERT_EQ(uid1, uid2);
    ASSERT_NE(uid1, uid3);
    ASSERT_EQ(uid4, uid2);
    ASSERT_LT(uid3, uid1);
}
