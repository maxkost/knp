#include <knp/core/device.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace core = knp::core;

/*
BOOST_PYTHON_MODULE(Device)
{
    py::enum_<core::DeviceType>("DeviceType")
        .value("CPU", DeviceType::0)
        .value("GPU", DeviceType::1)
        .value("AltAI1_NPU", DeviceType::2);

    py::class_<core::Device>("Device", "The Device class is the base class for devices supported by the device library.");
}
*/
