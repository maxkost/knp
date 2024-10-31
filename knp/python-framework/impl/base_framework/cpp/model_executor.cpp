/**
 * @file model_executor.h
 * @brief Model executor interface.
 * @kaspersky_support Artiom N.
 * @date 21.04.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.h"

#ifdef KNP_IN_BASE_FW
py::class_<knp::framework::ModelExecutor, boost::noncopyable>(
    "ModelExecutor",
    "The ModelExecutor class is a definition of an executor that runs the uploaded model on the specified backend.",
    py::no_init);
// .def("start", &knp::framework::ModelExecutor::start<void, void>, "Start model execution.")
// .def(
//     "start", &knp::framework::ModelExecutor::start<void, knp::core::Backend::run_predicate>,
//     "Start model execution.")
// .def("stop", &knp::framework::ModelExecutor::stop, "Stop model execution.")
// .def(
//     "get_output_channel", &knp::framework::ModelExecutor::get_output_channel,
//     "Get output channel.")  // TODO overloaded
// .def("get_input_channel", &knp::framework::ModelExecutor::get_input_channel, "Get input channel.")
// .def("start_learning", &knp::framework::ModelExecutor::start_learning, "Unlock synapse weights.")
// .def("stop_learning", &knp::framework::ModelExecutor::stop_learning, "Lock synapse weights.")
// .def("get_backend", &knp::framework::ModelExecutor::get_backend, "Get pointer to backend object.");

#endif  // KNP_IN_BASE_FW
