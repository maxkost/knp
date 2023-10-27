#include <knp/core/core.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace core = knp::core;


/*
BOOST_PYTHON_MODULE(Core)
{
    py::class_<core::TagMap>("TagMap", "The TagMap class is a definition of tags used by entity and their values.")
        .def(
            "get_tag", (std::any(core::TagMap::*)(std::string)) & core::TagMap::get_tag, "Get tag value by tag name.")
        .def("get_tag", &core::TagMap::get_tag, "Get tag value by tag name.")
        .def(
            "get_tag", (std::decay_t<T>(core::TagMap::*)(std::string)) & core::TagMap::get_tag,
            "Get tag value by tag name and value type.")
        .def("get_tag", &core::TagMap::get_tag, "Get tag value by tag name and value type.");

    py::class_<core::BaseData>("BaseData", "Common parameters for several different entities.")
}
*/
