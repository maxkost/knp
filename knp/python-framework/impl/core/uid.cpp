#include "common.h"

#if defined(__KNP_IN_CORE)

py::class_<core::continuously_uid_generator>("continuously_uid_generator", "UID generator for entities.");

py::class_<core::UID>("UID", "The UID class is a definition of unique identifiers for entities.")
    .def(py::init<bool>(py::arg("random")))  // , "Generate a random UID."
    .def(py::init<>())
    .def(py::init<const boost::uuids::uuid>())  // "Create a UID from `boost::uuids::uuid`."
    .def(py::init<boost::uuids::uuid>())        // "Create a UID from `boost::uuids::uuid`."
    .def(py::init<std::array<
             boost::uuids::uuid::value_type,
             sizeof(boost::uuids::uuid::data) / sizeof(boost::uuids::uuid::data[0])>>())  // "Create a UID from an array
                                                                                          // of 16 uint8_t integers."
    .def(py::init<core::UID>())  // "Constructor that creates a copy of an existing UID.")
    //        .def("uuid&", ((core::UID::*)()) & core::UID::uuid, "Convert UID to `boost::uuids::uuid`.")
    .def(py::self_ns::str(py::self))
    .def(
        "repr", make_handler(
                    [](const core::UID& self)
                    {
                        std::stringstream s;
                        s << "UID(" << self << ")";  // reinterpret_cast<const void*>(this);
                        return s.str();
                    }))
    .def("__bool__", &core::UID::operator bool, "Check if UID is valid.")
    // cppcheck-suppress syntaxError
    .def("__lt__", &core::UID::operator<, "Check if the current UIDgis less than the specified UID.")
    .def("__eq__", &core::UID::operator==, "Check if two UIDs are the same.")
    .def("__ne__", &core::UID::operator!=, "Check if two UIDs are different.")
    .def_readwrite("tag", &core::UID::tag, "UID value.");

py::class_<core::uid_hash>("uid_hash", "UID hash functor type.")
    .def("operator()", &core::uid_hash::operator(), "Get a hash value of the specified UID.");

#endif
