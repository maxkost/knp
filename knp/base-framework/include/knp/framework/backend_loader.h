/**
 * @file backend_loader.h
 * @brief Backend loader interface.
 * @author Artiom N.
 * @date 16.03.2023
 */

#pragma once

#include <knp/core/backend.h>

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>


/**
 * @brief Framework namespace.
 */
namespace knp::framework
{

/**
 * @brief The BackendLoader class is a definition of a backend loader.
 */
class BackendLoader
{
public:
    /**
     * @brief Load backend.
     * @param backend_path path to backend.
     * @return shared pointer to Backend object.
     */
    std::shared_ptr<core::Backend> load(const std::filesystem::path &backend_path);
    /**
     * @brief Check if the specified path points to a backend.
     * @param backend_path path to backend.
     * @return `true` if the library pointed by `backend_path` is a backend.
     */
    static bool is_backend(const std::filesystem::path &backend_path);

public:
    /**
     * @brief Function type that returns a backend instance.
     * @return shared pointer to Backend object.
     */
    typedef std::shared_ptr<core::Backend>(BackendCreateFunction)();

protected:
    /**
     * @brief Create a backend loader that will load and create a backend instance.
     * @param backend_path path to backend.
     * @return pointer to the backend creation function.
     */
    std::function<BackendCreateFunction> make_creator(const std::filesystem::path &backend_path);

private:
    // std::filesystem::path doesn't work on any compilers.
    std::unordered_map<std::string, std::function<BackendCreateFunction>> creators_;
};

}  // namespace knp::framework
