/**
 * @file common.h
 * @brief Python bindings common header for framework traits.
 * @kaspersky_support Artiom N.
 * @date 21.02.2024
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
#pragma once

#include <knp/framework/backend_loader.h>

#include <string>

#include <boost/python.hpp>
#include <boost/python/implicit.hpp>


namespace py = boost::python;
namespace cpp_framework = knp::framework;
