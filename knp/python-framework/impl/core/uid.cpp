#include <knp/core/uid.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace core = knp::core;


BOOST_PYTHON_MODULE(Uid)
{
    py::class_<core::continuously_uid_generator>("continuously_uid_generator", "UID generator for entities.");

    py::class_<core::UID>("UID", "The UID class is a definition of unique identifiers for entities.")
        .def(py::init<bool>()) /*, "Generate a random UID.")
        .def(py::init<>(), "Default UID constructor.")
        .def(py::init<const boost::uuids::uuid>(), "Create a UID from `boost::uuids::uuid`.")
        .def(py::init<boost::uuids::uuid>(), "Create a UID from `boost::uuids::uuid`.")
        .def(
            py::init<std::array<
                boost::uuids::uuid::value_type,
                sizeof(boost::uuids::uuid::data) / sizeof(boost::uuids::uuid::data[0])>>(),
            "Create a UID from an array of 16 uint8_t integers.")
        .def(py::init<core::UID>(), "Constructor that creates a copy of an existing UID.")*/
//        .def("uuid&", ((core::UID::*)()) & core::UID::uuid, "Convert UID to `boost::uuids::uuid`.")
//        .def(py::bool_(py::self), "Check if UID is valid.")
//        .def("operator<", &core::UID::operator<(), "Check if the current UIDgis less than the specified UID.")
//        .def(py::self == py::self, "Check if two UIDs are the same.")
//        .def(py::self != py::self, "Check if two UIDs are different.");
;

    py::class_<core::uid_hash>("uid_hash", "UID hash functor type.")
        .def("operator()", &core::uid_hash::operator(), "Get a hash value of the specified UID.");
}
