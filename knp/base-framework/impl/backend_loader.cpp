/**
 * @file backend_loader.cpp
 * @brief Backends loader implementation.
 * @author Artiom N.
 * @date 16.03.2023
 */

#include <knp/framework/backend_loader.h>

#include <spdlog/spdlog.h>

#include <functional>

#include <boost/dll/import.hpp>


namespace knp::framework
{

std::shared_ptr<core::Backend> BackendLoader::load(const std::filesystem::path &backend_path)
{
    typedef std::shared_ptr<core::Backend>(BackendCreateFunction)();

    SPDLOG_INFO("Loading backend by path \"{}\"", backend_path.string());

    std::function<BackendCreateFunction> creator = boost::dll::import_alias<BackendCreateFunction>(
        boost::filesystem::path(backend_path), "create_knp_backend", boost::dll::load_mode::append_decorations);

    return creator();
}


bool BackendLoader::is_backend(const std::filesystem::path &backend_path) const
{
    SPDLOG_INFO("Checking library by path \"{}\"", backend_path.string());
    boost::dll::shared_library lib{boost::filesystem::path(backend_path), boost::dll::load_mode::append_decorations};
    return lib.has("create_knp_backend");
}

}  // namespace knp::framework
