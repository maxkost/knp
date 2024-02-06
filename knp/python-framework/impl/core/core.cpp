#include <knp/core/core.h>

#include "common.h"


BOOST_PYTHON_MODULE(libknp_python_framework)
{
    //     py::class_<core::TagMap>("TagMap", "The TagMap class is a definition of tags used by entity and their
    //     values.")
    // //        .def(
    // //            "get_tag", (std::any(core::TagMap::*)(const std::string&))(&core::TagMap::get_tag), "Get tag value
    // by tag name.")
    //         // .def("get_tag", &core::TagMap::get_tag, "Get tag value by tag name.")
    //         .def(
    //             "get_tag", (std::string&(core::TagMap::*)(const std::string&))(&core::TagMap::get_tag),
    //             "Get tag value by tag name and value type.")
    // //        .def("get_tag", &core::TagMap::get_tag, "Get tag value by tag name and value type.");
    // ;

    py::class_<core::BaseData>("BaseData", "Common parameters for several different entities.");

#define __KNP_IN_CORE
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
#include "synaptic_index.cpp"           // NOLINT
#include "uid.cpp"                      // NOLINT
#undef __KNP_IN_CORE
}
