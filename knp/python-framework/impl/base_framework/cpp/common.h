/**
 * @file common.h
 * @brief Python bindings common header for framework traits.
 * @author Artiom N.
 * @date 21.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/uid.h>
#include <knp/framework/backend_loader.h>
#include <knp/framework/io/input_channel.h>
#include <knp/framework/io/output_channel.h>
#include <knp/framework/model.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/network.h>
#include <knp/framework/sonata/network_io.h>

#include <string>

#include <boost/python.hpp>
#include <boost/python/implicit.hpp>


namespace py = boost::python;
namespace cpp_framework = knp::framework;

template <class Entity>
knp::core::UID get_entity_uid(Entity &self)
{
    return self.get_uid();
}

// template <typename R, typename C, typename... Args>
// struct function_traits<R (C::*)(Args...) const>
//{
//     using function_type = std::function<R(Args...)>;
// };
// }  // namespace detail
//
//
// template <typename F>
// using function_type_t = typename detail::function_traits<F>::function_type;
//
// template <typename F>
// function_type_t<F> to_function(F lambda)
//{
//     return static_cast<function_type_t<F>>(lambda);
// }
//
//
// template <typename R, typename... Args>
// py::object make_handler(std::function<R(Args...)> func)
//{
//     return py::detail::make_function_aux(
//             func, py::default_call_policies(), boost::mpl::vector<std::decay_t<R>, Args...>());
// }
//
//
// template <typename F>
// py::object make_handler(F func)
//{
//     return make_handler(to_function(func));
// }
//
//
// template <typename T>
// std::vector<T> py_iterable_to_vector(const py::object &iterable)
//{
//     return std::vector<T>(py::stl_input_iterator<T>(iterable), py::stl_input_iterator<T>());
// }
