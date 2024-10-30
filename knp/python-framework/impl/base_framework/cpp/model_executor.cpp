/**
 * @file model_executor.h
 * @brief Model executor interface.
 * @author Artiom N.
 * @date 21.04.2023
 */

#include "common.h"

#ifdef KNP_IN_BASE_FW
py::class_<ModelExecutor>(
    "ModelExecutor",
    "The ModelExecutor class is a definition of an executor that runs the uploaded model on the specified backend.")
    .def("start", &knp::framework::ModelExecutor::start<void, void>, "Start model execution.")
    .def(
        "start", &knp::framework::ModelExecutor::start<void, knp::core::Backend::run_predicate>,
        "Start model execution.")
    .def("stop", &knp::framework::ModelExecutor::stop, "Stop model execution.")
    .def(
        "get_output_channel", &knp::framework::ModelExecutor::get_output_channel,
        "Get output channel.")  // TODO overloaded
    .def("get_input_channel", &knp::framework::ModelExecutor::get_input_channel, "Get input channel.")
    .def("start_learning", &knp::framework::ModelExecutor::start_learning, "Unlock synapse weights.")
    .def("stop_learning", &knp::framework::ModelExecutor::stop_learning, "Lock synapse weights.")
    .def("get_backend", &knp::framework::ModelExecutor::get_backend, "Get pointer to backend object.");

#endif  // KNP_IN_BASE_FW
