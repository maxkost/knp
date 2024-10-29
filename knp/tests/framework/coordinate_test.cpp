/**
 * @file coordinate_test.cpp
 * @brief Tests for coordinates.
 * @author Artiom N.
 * @date 22.03.2023.
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <knp/framework/coordinates/cartesian.h>
#include <knp/framework/coordinates/polar.h>

#include <tests_common.h>


TEST(FrameworkSuite, Carthesian2D)
{
    knp::framework::coordinates::cartesian::d2::coordinate<int> point(0, 2);

    ASSERT_EQ(point.x(), 0);
    ASSERT_EQ(point.y(), 2);
}


TEST(FrameworkSuite, Carthesian3D)
{
    knp::framework::coordinates::cartesian::d3::coordinate<double> point(1.1, 3.0, 2);

    ASSERT_DOUBLE_EQ(point.x(), 1.1);
    ASSERT_DOUBLE_EQ(point.y(), 3);
    ASSERT_DOUBLE_EQ(point.z(), 2);
}


TEST(FrameworkSuite, Polar)
{
    knp::framework::coordinates::radial::d2::coordinate<float> point(2.1, 3.0);

    ASSERT_FLOAT_EQ(point.radius(), 2.1);
    ASSERT_FLOAT_EQ(point.phi(), 3);
}


TEST(FrameworkSuite, Spherical)
{
    knp::framework::coordinates::radial::d3::coordinate<size_t> point(1, 3, 7);

    ASSERT_EQ(point.radius(), 1);
    ASSERT_EQ(point.theta(), 3);
    ASSERT_EQ(point.phi(), 7);

    point.theta(2);

    ASSERT_EQ(point.theta(), 2);
}
