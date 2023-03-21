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
#include <unordered_map>


/// Framework namespace.
namespace knp::framework
{

class BackendLoader
{
public:
    std::shared_ptr<core::Backend> load(const std::filesystem::path &backend_path);
    bool is_backend(const std::filesystem::path &backend_path) const;

public:
    typedef std::shared_ptr<core::Backend>(BackendCreateFunction)();

protected:
    std::function<BackendCreateFunction> make_creator(const std::filesystem::path &p);

private:
    std::unordered_map<std::filesystem::path, std::function<BackendCreateFunction>> creators_;
};

}  // namespace knp::framework
