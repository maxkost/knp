/**
 * @file backend_loader_test.cpp
 * @brief Backend loading testing.
 * @author Artiom N.
 * @date 17.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/backend_loader.h>

#include <tests_common.h>

#include <filesystem>


TEST(FrameworkSuite, BackendLoaderLoad)
{
    knp::framework::BackendLoader backend_loader;
    auto cpu_st_backend{backend_loader.load(knp::testing::get_backend_path())};

    EXPECT_NO_THROW((void)cpu_st_backend->get_uid());  // !OCLint(no "goto statement")
}


TEST(FrameworkSuite, BackendLoaderCheck)
{
    const knp::framework::BackendLoader backend_loader;

    ASSERT_TRUE(backend_loader.is_backend(knp::testing::get_backend_path()));
}


TEST(FrameworkSuite, BackendGetDevices)  // !OCLint(small method complexity)
{
    knp::framework::BackendLoader backend_loader;
    auto cpu_st_backend = backend_loader.load(knp::testing::get_backend_path());

    const std::set<knp::core::UID> dev_uids{knp::core::UID()};

    ASSERT_THROW(cpu_st_backend->select_devices(dev_uids), std::logic_error);  // !OCLint(no "goto statement")
}
