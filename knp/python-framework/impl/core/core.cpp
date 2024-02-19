/**
 * @file core.cpp
 * @brief Python core bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include <knp/core/core.h>

#include "any_converter.h"
#include "common.h"
#include "optional_converter.h"
#include "population.h"
#include "projection.h"

// knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>

// py::object my_tf(size_t index)
//{
//     std::cout << "Index ok = " << index << std::endl;
//     return py::object();
// }

BOOST_PYTHON_MODULE(KNP_FULL_LIBRARY_NAME)
{
    //    py::to_python_converter<std::any, to_python_any>();
    //    from_python_any();

    py::class_<knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>>(
        "BLIFATparameters", py::no_init);
    py::class_<core::TagMap>("TagMap", "The TagMap class is a definition of tags used by entity and their values.")
        //        .def("get_tag", static_cast<std::any&(core::TagMap::*)(const std::string&)>(&core::TagMap::get_tag),
        //             "Get tag value by tag name.")
        ;  // NOLINT

    py::class_<core::BaseData>("BaseData", "Common parameters for several different entities.");

    py::to_python_converter<std::optional<int>, to_python_optional<int>>();

#define _KNP_IN_CORE
#include "backend.cpp"                  // NOLINT
#include "device.cpp"                   // NOLINT
#include "message_bus.cpp"              // NOLINT
#include "message_endpoint.cpp"         // NOLINT
#include "message_header.cpp"           // NOLINT
#include "messaging.cpp"                // NOLINT
#include "population.cpp"               // NOLINT
#include "projection.cpp"               // NOLINT
#include "spike_message.cpp"            // NOLINT
#include "subscription.cpp"             // NOLINT
#include "synaptic_impact_message.cpp"  // NOLINT
#include "uid.cpp"                      // NOLINT
#undef __KNP_IN_CORE
}
