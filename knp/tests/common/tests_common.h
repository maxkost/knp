#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gtest/internal/gtest-filepath.h>

#include <filesystem>
#include <string>
#include <vector>


namespace testing::internal
{
/// Command line arguments.
::std::vector<std::string> GetArgvs();

#if GTEST_HAS_FILE_SYSTEM
//// Returns the current application's name, removing directory path if that
//// is present.
FilePath GetCurrentExecutableName();
#endif
}  // namespace testing::internal


namespace knp::testing
{

/// Return tester executable path.
std::filesystem::path get_exe_path();

/// Return backend path.
std::filesystem::path get_backend_path(const std::string &backend_name = "knp-cpu-single-threaded-backend");


}  // namespace knp::testing
