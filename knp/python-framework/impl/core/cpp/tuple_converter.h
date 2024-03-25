/**
 * @file tuple_converter.h
 * @brief C++ <-> Python std::tuple converter.
 * @author Roman Yakovenko copyright 2004-2007
 * @authors Artiom N.
 * @copyright Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 * @date 23.02.2024
 */

#pragma once

#include <tuple>

#include <boost/mpl/int.hpp>
#include <boost/mpl/next.hpp>
#include <boost/python/object.hpp>

/**
 * Converts std::tuples::tuple<...> to\from Python tuple
 *
 * The conversion is done "on-the-fly", you should only register the conversion
 * with your tuple classes.
 * For example:
 *
 *  typedef std::tuples::tuple< int, double, std::string > triplet;
 *  boost::python::register_tuple< triplet >();
 *
 * That's all. After this point conversion to\from next types will be handled
 * by Boost.Python library:
 *
 *   triplet
 *   triplet& ( return type only )
 *   const triplet
 *   const triplet&
 *
 * Implementation description.
 *  The conversion uses Boost.Python custom r-value converters. r-value converters
 * is very powerful and undocumented feature of the library. The only documentation
 * we have is http://boost.org/libs/python/doc/v2/faq.html#custom_string .
 *
 *  The conversion consists from two parts: "to" and "from".
 *
 *  "To" conversion
 * The "to" part is pretty easy and well documented ( http://docs.python.org/api/api.html ).
 * You should use Python C API to create an instance of a class and than you
 * initialize the relevant members of the instance.
 *
 *  "From" conversion
 * Lets start from analyzing one of the use case Boost.Python library have to
 * deal with:
 *
 *   void do_smth( const triplet& arg ){...}
 *
 * In order to allow calling this function from Python, the library should keep
 * parameter "arg" alive until the function returns. In other words, the library
 * should provide instances life-time management. The provided interface is not
 * ideal and could be improved. You have to implement two functions:
 *
 *  void* convertible( PyObject* obj )
 *    Checks whether the "obj" could be converted to an instance of the desired
 *    class. If true, the function should return "obj", otherwise NULL
 *
 *  void construct( PyObject* obj, converter::rvalue_from_python_stage1_data* data)
 *    Constructs the instance of the desired class. This function will be called
 *    if and only if "convertible" function returned true. The first argument
 *    is Python object, which was passed as parameter to "convertible" function.
 *    The second object is some kind of memory allocator for one object. Basically
 *    it keeps a memory chunk. You will use the memory for object allocation.
 *
 *    For some unclear for me reason, the library implements "C style Inheritance"
 *    ( http://www.embedded.com/97/fe29712.htm ). So, in order to create new
 *    object in the storage you have to cast to the "right" class:
 *
 *      typedef converter::rvalue_from_python_storage<your_type_t> storage_t;
 *      storage_t* the_storage = reinterpret_cast<storage_t*>( data );
 *      void* memory_chunk = the_storage->storage.bytes;
 *
 *    "memory_chunk" points to the memory, where the instance will be allocated.
 *
 *    In order to create object at specific location, you should use placement new
 *    operator:
 *
 *      your_type_t* instance = new (memory_chunk) your_type_t();
 *
 *    Now, you can continue to initialize the instance.
 *
 *      instance->set_xyz = read xyz from obj
 *
 *    If "your_type_t" constructor requires some arguments, "read" the Python
 *    object before you call the constructor:
 *
 *      xyz_type xyz = read xyz from obj
 *      your_type_t* instance = new (memory_chunk) your_type_t(xyz);
 *
 *  Hint:
 * In most case you don't really need\have to work with C Python API. Let
 * Boost.Python library to do some work for you!
 *
 **/

namespace boost
{
namespace python
{

namespace details
{

// Small helper function, introduced to allow short syntax for index incrementing
template <int index>
typename mpl::next<mpl::int_<index> >::type increment_index()
{
    using next_index_type = typename mpl::next<mpl::int_<index> >::type;
    return next_index_type();
}

}  // namespace details

template <class TTuple>
struct to_py_tuple
{
    using length_type = mpl::int_<std::tuple_size<TTuple>::value>;

    static PyObject* convert(const TTuple& c_tuple)
    {
        list values;
        // add all c_tuple items to "values" list
        convert_impl(c_tuple, values, mpl::int_<0>(), length_type());
        // create Python tuple from the list
        return incref(python::tuple(values).ptr());
    }

private:
    template <int index, int length>
    static void convert_impl(const TTuple& c_tuple, list& values, mpl::int_<index>, mpl::int_<length>)
    {
        values.append(std::get<index>(c_tuple));
        convert_impl(c_tuple, values, details::increment_index<index>(), length_type());
    }

    template <int length>
    static void convert_impl(const TTuple&, list&, mpl::int_<length>, mpl::int_<length>)
    {
    }
};


template <class TTuple>
struct from_py_sequence
{
    using length_type = mpl::int_<std::tuple_size<TTuple>::value>;

    static void* convertible(PyObject* py_obj)
    {
        if (!PySequence_Check(py_obj))
        {
            return nullptr;
        }

        if (!PyObject_HasAttrString(py_obj, "__len__"))
        {
            return nullptr;
        }

        python::object py_sequence(handle<>(borrowed(py_obj)));

        if (std::tuple_size<TTuple>::value != len(py_sequence))
        {
            return nullptr;
        }

        if (convertible_impl(py_sequence, mpl::int_<0>(), length_type()))
        {
            return py_obj;
        }

        return nullptr;
    }

    static void construct(PyObject* py_obj, converter::rvalue_from_python_stage1_data* data)
    {
        auto the_storage = reinterpret_cast<converter::rvalue_from_python_storage<TTuple>*>(data);
        auto memory_chunk = the_storage->storage.bytes;
        auto c_tuple = new (memory_chunk) TTuple();
        data->convertible = memory_chunk;

        python::object py_sequence(handle<>(borrowed(py_obj)));
        construct_impl(py_sequence, *c_tuple, mpl::int_<0>(), length_type());
    }

    static TTuple to_c_tuple(PyObject* py_obj)
    {
        if (!convertible(py_obj))
        {
            throw std::runtime_error("Unable to construct boost::tuples::tuple from Python object!");
        }
        TTuple c_tuple;
        python::object py_sequence(handle<>(borrowed(py_obj)));
        construct_impl(py_sequence, c_tuple, mpl::int_<0>(), length_type());
        return c_tuple;
    }

private:
    template <int index, int length>
    static bool convertible_impl(const python::object& py_sequence, mpl::int_<index>, mpl::int_<length>)
    {
        using element_type = std::tuple_element_t<index, TTuple>;

        object element = py_sequence[index];
        extract<element_type> type_checker(element);
        if (!type_checker.check())
        {
            return false;
        }

        return convertible_impl(py_sequence, details::increment_index<index>(), length_type());
    }

    template <int length>
    static bool convertible_impl(const python::object&, mpl::int_<length>, mpl::int_<length>)
    {
        return true;
    }

    template <int index, int length>
    static void construct_impl(const python::object& py_sequence, TTuple& c_tuple, mpl::int_<index>, mpl::int_<length>)
    {
        using element_type = std::tuple_element_t<index, TTuple>;

        object element = py_sequence[index];
        std::get<index>(c_tuple) = extract<element_type>(element);

        construct_impl(py_sequence, c_tuple, details::increment_index<index>(), length_type());
    }

    template <int length>
    static void construct_impl(const python::object&, TTuple&, mpl::int_<length>, mpl::int_<length>)
    {
    }
};


template <class TTuple>
void register_tuple()
{
    to_python_converter<TTuple, to_py_tuple<TTuple> >();

    converter::registry::push_back(
        &from_py_sequence<TTuple>::convertible, &from_py_sequence<TTuple>::construct, type_id<TTuple>());
};

}  // namespace python
}  // namespace boost
