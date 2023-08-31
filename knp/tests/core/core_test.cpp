/**
 * UUID tests.
 */

#include <knp/core/core.h>

#include <tests_common.h>


TEST(CoreSuite, TagMapTest)
{
    knp::core::TagMap tm;

    tm.get_tag("test") = std::string("test_val");

    ASSERT_EQ(tm.get_tag<std::string>("test"), "test_val");
}
