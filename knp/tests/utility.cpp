/**
 * @file utility.cpp
 * @brief Some common test functions.
 * @author Artiom N.
 * @date 20.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
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
