/**
 * @brief Some common test functions.
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
