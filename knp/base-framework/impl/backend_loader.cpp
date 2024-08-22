/**
 * @file backend_loader.cpp
 * @brief Backends loader implementation.
 * @author Artiom N.
 * @date 16.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/backend_loader.h>

#include <spdlog/spdlog.h>

#include <functional>

#include <boost/dll/import.hpp>


namespace knp::framework
{

std::function<BackendLoader::BackendCreateFunction> BackendLoader::make_creator(
    const std::filesystem::path &backend_path)
{
    auto creator_iter = creators_.find(backend_path.string());

    if (creator_iter != creators_.end())
    {
        return creator_iter->second;
    }

    SPDLOG_INFO("Loading backend by path \"{}\"", backend_path.string());

    auto creator = boost::dll::import_alias<BackendCreateFunction>(
        boost::filesystem::path(backend_path), "create_knp_backend", boost::dll::load_mode::append_decorations);

    SPDLOG_DEBUG("Created backend creator...");

    creators_[backend_path.string()] = creator;

    return creator;
}


std::shared_ptr<core::Backend> BackendLoader::load(const std::filesystem::path &backend_path)
{
    auto creator = make_creator(backend_path);
    SPDLOG_INFO("Create backend instance...");
    return creator();
}


bool BackendLoader::is_backend(const std::filesystem::path &backend_path)
{
    SPDLOG_INFO("Checking library by path \"{}\"", backend_path.string());
    const boost::dll::shared_library lib{
        boost::filesystem::path(backend_path), boost::dll::load_mode::append_decorations};
    return lib.has("create_knp_backend");
}

}  // namespace knp::framework
