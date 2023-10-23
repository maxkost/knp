#include <knp/core-library/include/knp/core/core.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(Core)
{
    py::class_<TagMap>("TagMap", "The TagMap class is a definition of tags used by entity and their values.")
        .def(
            "get_tag", ([[nodiscard]] std::any(TagMap::*)(std::string)) & TagMap::get_tag, "Get tag value by tag name.")
        .def("get_tag", &TagMap::get_tag, "Get tag value by tag name.")
        .def(
            "get_tag", ([[nodiscard]] std::decay_t<T>(TagMap::*)(std::string)) & TagMap::get_tag,
            "Get tag value by tag name and value type.")
        .def("get_tag", &TagMap::get_tag, "Get tag value by tag name and value type.");

    py::class_<BaseData>("BaseData", "Common parameters for several different entities.")
}
