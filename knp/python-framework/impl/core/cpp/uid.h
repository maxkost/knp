/**
 * @file uid.h
 * @brief Python bindings header for UID.
 * @kaspersky_support Artiom N.
 * @date 22.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <vector>

#include "common.h"


static py::object py_uuid_UUID;


struct uid_into_python
{
    static PyObject* convert(boost::uuids::uuid const& u)
    {
        return py::incref(py_uuid_UUID(py::object(), std::vector<uint8_t>(u.begin(), u.end())).ptr());
    }
};


struct uid_from_python
{
    uid_from_python() { py::converter::registry::push_back(convertible, construct, py::type_id<boost::uuids::uuid>()); }

    static void* convertible(PyObject* obj) { return PyObject_IsInstance(obj, py_uuid_UUID.ptr()) ? obj : nullptr; }

    static void construct(PyObject* obj, py::converter::rvalue_from_python_stage1_data* data)
    {
        auto storage =
            reinterpret_cast<py::converter::rvalue_from_python_storage<boost::uuids::uuid>*>(data)->storage.bytes;
        std::vector<uint8_t> ba =
            py::extract<std::vector<uint8_t>>(py::object(py::handle<>(py::borrowed(obj))).attr("bytes"));
        boost::uuids::uuid* res = new (storage) boost::uuids::uuid;

        memcpy(res->data, &ba.front(), ba.size());
        data->convertible = storage;
    }
};
