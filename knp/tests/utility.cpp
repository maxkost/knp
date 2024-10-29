/**
 * @file utility.cpp
 * @brief Some common test functions.
 * @author Artiom N.
 * @date 20.03.2023
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

#include <tests_common.h>


namespace knp::testing
{

std::filesystem::path get_exe_path()
{
    return std::filesystem::weakly_canonical(std::filesystem::path(::testing::internal::GetArgvs()[0])).parent_path();
}


std::filesystem::path get_backend_path(const std::string &backend_name)
{
    return knp::testing::get_exe_path() / backend_name;
}

}  // namespace knp::testing
