/**
 * UUID tests.
 */

#include <knp/core/uid.h>

#include <string>

#include "tests_common.h"


TEST(UidTest, UidGeneratorTest)
{
    auto u = ::knp::core::continuously_uid_generator()();
    ASSERT_EQ(uuid{{1}}, u);
    ASSERT_NE(uuid{{2}}, u);

    auto u_gen = ::knp::core::continuously_uid_generator();

    for (uint8_t i = 2; i < 100; ++i) ASSERT_EQ(uuid{{i}}, u_gen());
}


TEST(UidTest, UidToStringTest)
{
    ::knp::core::UID uid{::boost::uuids::uuid{{1, 2, 3}}};

    ASSERT_EQ("01020300-0000-0000-0000-000000000000", std::string(uid));

    std::cout << uid << std::endl;
}
