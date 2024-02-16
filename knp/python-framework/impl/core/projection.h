/**
 * @file projection.h
 * @brief Python bindings header for Projection.
 * @author Artiom N.
 * @date 16.02.2024
 */
#pragma once

#include <knp/core/projection.h>
#include <knp/synapse-traits/all_traits.h>

#include <iostream>
#include <memory>
#include <optional>
#include <tuple>

#include "common.h"


template <typename ElemParametersType>
struct ProjectionGeneratorProxy
{
    explicit ProjectionGeneratorProxy(const py::object &gen_func) : gen_func_(gen_func) {}

    std::optional<std::tuple<ElemParametersType, size_t, size_t>> operator()(size_t index)
    {
        auto res = py::call<py::object>(gen_func_.ptr(), index);
        if (res.is_none()) return std::nullopt;
        return *reinterpret_cast<std::tuple<ElemParametersType, size_t, size_t> *>(res.ptr());
    }

    const py::object &gen_func_;
};


// TODO: remove this.
template <typename ElemType>
struct ProjectionGeneratorProxy1
{
    explicit ProjectionGeneratorProxy1(const py::object &gen_func) : gen_func_(gen_func) {}

    std::optional<typename core::Projection<ElemType>::Synapse> operator()(size_t index)
    {
        auto res = py::call<py::object>(gen_func_.ptr(), index);
        if (res.is_none()) return std::nullopt;
        return *reinterpret_cast<typename core::Projection<ElemType>::Synapse *>(res.ptr());
    }

    const py::object &gen_func_;
};


template <typename ElemType>
std::shared_ptr<core::Projection<ElemType>> projection_constructor_wrapper(
    core::UID uid, core::UID presynaptic_uid, core::UID postsynaptic_uid, const py::object &gen_func,
    size_t num_iterations)
{
    return std::make_shared<core::Projection<ElemType>>(
        uid, presynaptic_uid, postsynaptic_uid,
        ProjectionGeneratorProxy<typename core::Projection<ElemType>::SynapseParameters>(gen_func), num_iterations);
};


template <typename ElemType>
std::shared_ptr<core::Projection<ElemType>> projection_constructor_wrapper(
    core::UID presynaptic_uid, core::UID postsynaptic_uid, const py::object &gen_func, size_t num_iterations)
{
    return std::make_shared<core::Projection<ElemType>>(
        presynaptic_uid, postsynaptic_uid,
        ProjectionGeneratorProxy<typename core::Projection<ElemType>::SynapseParameters>(gen_func), num_iterations);
};


template <typename ElemType>
void projection_synapses_add_wrapper(
    core::Projection<ElemType> &projection, const py::object &gen_func, size_t num_iterations)
{
    projection.add_synapses(ProjectionGeneratorProxy1<ElemType>(gen_func), num_iterations);
};
