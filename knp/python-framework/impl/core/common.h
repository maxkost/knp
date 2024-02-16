/**
 * @file common.h
 * @brief Python bindings common header.
 * @author Artiom N.
 * @date 05.02.2024
 */
#pragma once

#include <knp/core/device.h>
#include <knp/core/message_bus.h>
#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/message_envelope.h>
#include <knp/core/messaging/message_header.h>
#include <knp/core/messaging/messaging.h>
#include <knp/core/messaging/synaptic_impact_message.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/core/synaptic_index.h>
#include <knp/core/uid.h>

#include <sstream>

#include <boost/python.hpp>


namespace py = boost::python;
namespace core = knp::core;
namespace nt = knp::neuron_traits;
namespace st = knp::synapse_traits;


namespace detail
{
template <typename F>
struct function_traits : public function_traits<decltype(&F::operator())>
{
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const>
{
    using function_type = std::function<R(Args...)>;
};
}  // namespace detail

template <typename F>
using function_type_t = typename detail::function_traits<F>::function_type;

template <typename F>
function_type_t<F> to_function(F lambda)
{
    return static_cast<function_type_t<F>>(lambda);
}


template <typename R, typename... Args>
py::object make_handler(std::function<R(Args...)> func)
{
    return py::detail::make_function_aux(
        func, py::default_call_policies(), boost::mpl::vector<std::decay_t<R>, Args...>());
}


template <typename F>
py::object make_handler(F func)
{
    return make_handler(to_function(func));
}
