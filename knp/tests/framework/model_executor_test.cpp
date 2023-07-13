/**
 * Backend loading testing.
 */

#include <knp/framework/model_executor.h>

#include <tests_common.h>

#include <filesystem>


TEST(FrameworkSuite, ModelExecutorLoad)
{
    knp::framework::BackendLoader backend_loader;
    auto cpu_st_backend{backend_loader.load(knp::testing::get_backend_path())};
    EXPECT_NO_THROW((void)cpu_st_backend->get_uid());

    //    knp::framework::ModelExecutor::
}
