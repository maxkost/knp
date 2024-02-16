/**
 * @file uid.cpp
 * @brief Python bindings for UID.
 * @author Artiom N.
 * @date 05.02.2024
 */

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "common.h"


#if defined(__KNP_IN_CORE)

/*
namespace
{

boost::uuids::nil_generator nil_gen;
boost::uuids::string_generator parse_uuid;

struct boost_uuid_to_python
{
    static PyObject* convert(boost::uuids::uuid const& uuid)
    {
        std::string s(boost::uuids::to_string(uuid));
        PyObject* result = PyString_FromStringAndSize(s.data(), static_cast<Py_ssize_t>(s.length()));
        Py_INCREF(result);

        return result;
    }
};
}


struct boost_uuid_from_python
{
    boost_uuid_from_python()
    {
        py::converter::registry::push_back(&convertible, &construct, py::type_id<boost::uuids::uuid>());
    }

    static void* convertible(PyObject* obj)
    {
        if (obj == Py_None || PyUnicode_Check(obj) || PyString_Check(obj)) return obj;
        return nullptr;
    }

    static void construct(PyObject* obj, py::converter::rvalue_from_python_stage1_data* data)
    {
        typedef py::converter::rvalue_from_python_storage<boost::uuids::uuid> storage_type;

        boost::uuids::uuid uuid;

        if (Py_None == obj) uuid = nil_gen();
        else
        {
            PyObject* str = nullptr;
            const char* value = "";

            if (PyUnicode_Check(obj))
            {
                str = PyUnicode_AsUTF8String(obj);
                if (!str) python::throw_error_already_set();
                obj = str;
            }

            value = PyString_AsString(obj);
            Py_XDECREF(str);
            if (!value) py::throw_error_already_set();

            try
            {
                uuid = parse_uuid(value);
            }
            catch (const std::runtime_error& e)
            {
                PyErr_SetString(PyExc_ValueError, "Invalid UUID value.");
                py::throw_error_already_set();
            }
        }

        void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;
        new (storage) boost::uuids::uuid(uuid);
        data->convertible = storage;
    }
};
}

void exportUUUID()
{
    py::to_python_converter<boost::uuids::uuid, boost_uuid_to_python>();
    uuid_from_python();
}
*/

py::class_<core::continuously_uid_generator>("continuously_uid_generator", "UID generator for entities.");

py::class_<core::UID>("UID", "The UID class is a definition of unique identifiers for entities.")
    .def(py::init<bool>(py::arg("random")))  // , "Generate a random UID."
    .def(py::init<>())
    .def(py::init<const boost::uuids::uuid>())  // "Create a UID from `boost::uuids::uuid`."
    .def(py::init<boost::uuids::uuid>())        // "Create a UID from `boost::uuids::uuid`."
    // Create a UID from an array of 16 uint8_t integers.
    .def(py::init<std::array<
             boost::uuids::uuid::value_type,
             sizeof(boost::uuids::uuid::data) / sizeof(boost::uuids::uuid::data[0])>>())
    .def(py::init<core::UID>())  // "Constructor that creates a copy of an existing UID."
    //        .def("uuid&", static_cast<(core::UID::*)()>(&core::UID::uuid), "Convert UID to `boost::uuids::uuid`.")
    .def(py::self_ns::str(py::self))
    .def("__bool__", &core::UID::operator bool, "Check if UID is valid.")
    // cppcheck-suppress syntaxError
    .def("__lt__", &core::UID::operator<, "Check if the current UIDgis less than the specified UID.")
    .def("__eq__", &core::UID::operator==, "Check if two UIDs are the same.")
    .def("__ne__", &core::UID::operator!=, "Check if two UIDs are different.")
    .def_readwrite("tag", &core::UID::tag, "UID value.");

py::class_<core::uid_hash>("uid_hash", "UID hash functor type.")
    .def("operator()", &core::uid_hash::operator(), "Get a hash value of the specified UID.");

#endif
