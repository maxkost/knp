#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gtest/internal/gtest-filepath.h>

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
