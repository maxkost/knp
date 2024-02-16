/**
 * @file optional_converter.h
 * @brief std::optional for Python.
 * @author Artiom N.
 * @date 09.02.2024
 */

#pragma once

#include <object.h>

#include <vector>

#include "common.h"


// Custom exceptions
struct AttributeError : std::exception
{
    const char* what() const throw() { return "AttributeError exception"; }
};

struct TypeError : std::exception
{
    const char* what() const throw() { return "TypeError exception"; }
};

// Set python exceptions
void translate(const std::exception& e)
{
    // cppcheck-suppress knownPointerToBool
    if (dynamic_cast<const AttributeError*>(&e)) PyErr_SetString(PyExc_AttributeError, e.what());
    // cppcheck-suppress knownPointerToBool
    if (dynamic_cast<const TypeError*>(&e)) PyErr_SetString(PyExc_TypeError, e.what());
}


template <typename T>
struct to_python_optional
{
    static PyObject* convert(const std::optional<T>& obj)
    {
        if (obj) return py::incref(py::object(*obj).ptr());
        // Raise AttributeError if any value hasn't been set yet.
        else
            return nullptr;
    }
};


template <typename T>
struct from_python_optional
{
    static void* convertible(PyObject* obj_ptr)
    {
        try
        {
            return typename py::extract<T>::extract(obj_ptr) ? obj_ptr : nullptr;
        }
        // Without try catch it still raises a TypeError exception
        // But this enables to custom your error message
        catch (...)
        {
            throw TypeError();
        }
    }

    static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
        const T value = typename py::extract<T>::extract(obj_ptr);

        if (value)
        {
            void* storage = ((py::converter::rvalue_from_python_storage<std::optional<T>>*)data)->storage.bytes;
            new (storage) std::optional<T>(value);
            data->convertible = storage;
        }
        else
        {
            data->convertible = nullptr;
        }
    }

    from_python_optional()
    {
        py::converter::registry::push_back(&convertible, &construct, py::type_id<std::optional<T>>());
    }
};
