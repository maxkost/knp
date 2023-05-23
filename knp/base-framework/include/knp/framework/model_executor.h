/**
 * @file model_executor.h
 * @brief Model executor interface.
 * @author Artiom N.
 * @date 21.04.2023
 */

#pragma once

#include <knp/framework/backend_loader.h>
#include <knp/framework/model.h>

#include <filesystem>
#include <memory>


/**
 * @brief Framework namespace.
 */
namespace knp::framework
{

/**
 * @brief The ModelExecutor class is a definition of an executor that runs the uploaded model on the specified backend.
 */
class ModelExecutor
{
public:
    /**
     * @brief ModelExecutor constructor.
     * @param model model to run.
     * @param backend_path filepath to backend on which you want to run the model.
     */
    ModelExecutor(knp::framework::Model &model, const std::filesystem::path backend_path)
        : backend_loader_(), model_(model)
    {
        backend_ = backend_loader_.load(backend_path);
    }

public:
    /**
     * @brief Run model.
     */
    void run();

private:
    knp::core::BaseData base_;
    knp::framework::BackendLoader backend_loader_;
    std::shared_ptr<core::Backend> backend_;
    knp::framework::Model &model_;
};

}  // namespace knp::framework
