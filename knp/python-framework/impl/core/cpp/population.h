/**
 * @file population.h
 * @brief Python bindings header for Population.
 * @author Artiom N.
 * @date 07.02.2024
 */
#pragma once

#include <knp/core/population.h>
#include <knp/neuron-traits/all_traits.h>

#include <iostream>
#include <memory>
#include <optional>

#include "common.h"

template <typename ElemParametersType>
struct PopulationGeneratorProxy
{
    explicit PopulationGeneratorProxy(const py::object &gen_func) : gen_func_(gen_func)
    {
        if (!static_cast<bool>(PyCallable_Check(gen_func.ptr())))
        {
            PyErr_SetString(PyExc_TypeError, "Passed generator is not callable!");
            py::throw_error_already_set();
        }
    }

    std::optional<ElemParametersType> operator()(size_t index)
    {
        auto res = py::call<py::object>(gen_func_.ptr(), index);
        if (res.is_none())
        {
            return std::nullopt;
        }
        return py::extract<ElemParametersType>(res);
    }

    const py::object &gen_func_;
};


template <typename ElemType>
std::shared_ptr<core::Population<ElemType>> population_constructor_wrapper(
    const core::UID &uid, const py::object &gen_func, size_t index)
{
    return std::make_shared<core::Population<ElemType>>(
        uid, PopulationGeneratorProxy<typename core::Population<ElemType>::NeuronParameters>(gen_func), index);
};


template <typename ElemType>
std::shared_ptr<core::Population<ElemType>> population_constructor_wrapper(const py::object &gen_func, size_t index)
{
    return std::make_shared<core::Population<ElemType>>(
        PopulationGeneratorProxy<typename core::Population<ElemType>::NeuronParameters>(gen_func), index);
};


template <typename ElemType>
void population_neurons_add_wrapper(core::Population<ElemType> &population, const py::object &gen_func, size_t index)
{
    population.add_neurons(
        PopulationGeneratorProxy<typename core::Population<ElemType>::NeuronParameters>(gen_func), index);
};
