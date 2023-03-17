/**
 * Backend loading testing.
 */

#include <knp/framework/backend_loader.h>

#include <tests_common.h>

#include <filesystem>


TEST(FrameworkSuite, BackendLoader)
{
    knp::framework::BackendLoader backend_loader;
    auto exe_dir =
        std::filesystem::weakly_canonical(std::filesystem::path(testing::internal::GetArgvs()[0])).parent_path();

    auto cpu_st_backend = backend_loader.load(exe_dir.parent_path() / "lib" / "knp-cpu-single-threaded-backend");

    std::cout << cpu_st_backend->get_uid() << std::endl;
}
