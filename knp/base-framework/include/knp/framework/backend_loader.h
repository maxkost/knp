/**
 * @file backend_loader.h
 * @brief Backends loader interface.
 * @author Artiom N.
 * @date 16.03.2023
 */

#pragma once

#include <knp/core/backend.h>

#include <filesystem>
#include <memory>


/// Framework namespace.
namespace knp::framework
{

class BackendLoader
{
public:
    std::shared_ptr<core::Backend> load(const std::filesystem::path &backend_path);
    bool is_backend(const std::filesystem::path &backend_path) const;
};

}  // namespace knp::framework
