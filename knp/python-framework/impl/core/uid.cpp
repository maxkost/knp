/**
 * @file uid.cpp
 * @brief Python bindings for UID.
 * @author Artiom N.
 * @date 05.02.2024
 */

#include "uid.h"

#include "common.h"


#if defined(_KNP_IN_CORE)

py_uuid_UUID = py::import("uuid").attr("UUID");

py::implicitly_convertible<core::UID, boost::uuids::uuid>();

py::class_<core::continuously_uid_generator>("continuously_uid_generator", "UID generator for entities.");

py::enum_<boost::uuids::uuid::variant_type>("uuid_variant_type")
    .value("NCS", boost::uuids::uuid::variant_type::variant_ncs)
    .value("RFC_4122", boost::uuids::uuid::variant_type::variant_rfc_4122)
    .value("MICROSOFT", boost::uuids::uuid::variant_type::variant_microsoft)
    .value("FUTURE", boost::uuids::uuid::variant_type::variant_future);

py::class_<boost::uuids::uuid>("uuid", "Boost UUID")
    .add_property("size", &boost::uuids::uuid::size)
    .add_property("is_nil", &boost::uuids::uuid::is_nil)
    .add_property("variant", &boost::uuids::uuid::variant, "UID format variant");

py::to_python_converter<boost::uuids::uuid, uid_into_python>();
uid_from_python();

py::class_<core::UID>("UID", "The UID class is a definition of unique identifiers for entities.")
    .def(py::init<bool>(py::arg("random")))     // , "Generate a random UID."
    .def(py::init<const boost::uuids::uuid>())  // "Create a UID from `boost::uuids::uuid`."
    .def(py::init<boost::uuids::uuid>())        // "Create a UID from `boost::uuids::uuid`."
    // Create a UID from an array of 16 uint8_t integers.
    .def(py::init<std::array<
             boost::uuids::uuid::value_type,
             sizeof(boost::uuids::uuid::data) / sizeof(boost::uuids::uuid::data[0])>>())
    .def(py::init<core::UID>())  // "Constructor that creates a copy of an existing UID."
    .def(py::self_ns::str(py::self))
    .def("__bool__", &core::UID::operator bool, "Check if UID is valid.")
    // cppcheck-suppress syntaxError
    .def("__lt__", &core::UID::operator<, "Check if the current UIDgis less than the specified UID.")
    .def("__eq__", &core::UID::operator==, "Check if two UIDs are the same.")
    .def("__ne__", &core::UID::operator!=, "Check if two UIDs are different.")
    .add_property("tag", &core::UID::tag, "UID value.");

py::class_<core::uid_hash>("uid_hash", "UID hash functor type.")
    .def("operator()", &core::uid_hash::operator(), "Get a hash value of the specified UID.");

// py::class_<std::vector<core::UID>>("UIDs").def(py::vector_indexing_suite<core::UID>());

#endif
