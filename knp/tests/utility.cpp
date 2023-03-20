#include <tests_common.h>


namespace knp::testing
{

std::filesystem::path get_exe_path()
{
    return std::filesystem::weakly_canonical(std::filesystem::path(::testing::internal::GetArgvs()[0])).parent_path();
}

}  // namespace knp::testing
