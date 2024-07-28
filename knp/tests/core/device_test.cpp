/**
 * @brief Device tests.
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/devices/cpu.h>

#include <tests_common.h>


TEST(DeviceTestSuite, CPUTest)
{
    for (auto &device : knp::devices::cpu::list_processors())
        std::cout << "NAME ::: " << device.get_name() << std::endl;
}


TEST(DeviceTestSuite, BackendDevicesTest)
{
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
