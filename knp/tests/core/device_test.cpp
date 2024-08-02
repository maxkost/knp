/**
 * @brief Device tests.
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

extern "C"
{
#include <unistd.h>
}

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/devices/cpu.h>

#include <spdlog/spdlog.h>
#include <tests_common.h>


TEST(DeviceTestSuite, CPUTest)
{
    if (geteuid())
    {
        SPDLOG_WARN("This test must be ran under root");
        return;
    }

    for (auto &device : knp::devices::cpu::list_processors())
        std::cout << "NAME ::: " << device.get_name() << std::endl;
    //    knp::devices::cpu::CPU device;
}


TEST(DeviceTestSuite, BackendDevicesTest)
{
    if (geteuid())
    {
        SPDLOG_WARN("This test must be ran under root");
        return;
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
