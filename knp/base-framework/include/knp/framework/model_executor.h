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

namespace knp::framework
{

/**
 * @brief The ModelExecutor class.
 */
class ModelExecutor
{
public:
    ModelExecutor(knp::framework::Model &model, const std::filesystem::path backend_path)
        : backend_loader_(), model_(model)
    {
        backend_loader_.load(backend_path);
    }

public:
    /**
     * @brief run the model.
     */
    void run();

private:
    knp::core::BaseData base_;
    knp::framework::BackendLoader backend_loader_;
    knp::framework::Model &model_;
};

}  // namespace knp::framework
