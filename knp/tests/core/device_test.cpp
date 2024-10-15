/**
 * @file device_test.cpp
 * @brief Device tests.
 * @date 09.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#if !defined(_MSC_VER)
extern "C"
{
#    include <unistd.h>
}
#endif

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/devices/cpu.h>

#include <tests_common.h>

TEST(DeviceTestSuite, CPUTest)
{
#if defined(WIN32)
    GTEST_SKIP() << "Test needs driver under Windows";
#endif
    if (geteuid() != 0)
    {
        GTEST_SKIP() << "This test must be run under root.";
    }

    auto processors = knp::devices::cpu::list_processors();

    ASSERT_GE(processors.size(), 1);

    for (auto &device : processors) SPDLOG_DEBUG("CPU name: {}.", device.get_name());
}


TEST(DeviceTestSuite, BackendDevicesTest)
{
#if defined(WIN32)
    GTEST_SKIP() << "Test needs driver under Windows";
#endif
    if (geteuid() != 0)
    {
        SPDLOG_WARN("This test must be run under root.");
        GTEST_SKIP() << "This test must be run under root.";
    }

    knp::backends::single_threaded_cpu::SingleThreadedCPUBackend backend;

    auto &devices = backend.get_current_devices();

    std::set<knp::core::UID> dus;

    backend.select_devices(dus);

    for (auto &d : devices)
    {
        std::cout << d->get_name() << std::endl;
    }

    // std::cout << device.get_name() << std::endl;
}
