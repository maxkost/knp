/**
 * @file thread_pool.h
 * @brief Reusable thread pool class, a modified example from asio documentation.
 * @author Vartenkov A.
 * @date 27.07.2023
 */
#pragma once
#include <memory>

#include "thread_pool_context.h"
#include "thread_pool_executor.h"

/**
 * @brief Namespace for multi-threaded backend.
 */
namespace knp::backends::multi_threaded_cpu
{
class ThreadPool
{
public:
    explicit ThreadPool(size_t num_threads)
        : context_(std::make_unique<ThreadPoolContext>(num_threads)), executor_(*context_)
    {
    }

    template <class Func>
    void post(Func task)
    {
        boost::asio::post(executor_, task);
    }

    void join() { executor_.join(); }

private:
    std::unique_ptr<ThreadPoolContext> context_;
    ThreadPoolExecutor executor_;
};

}  // namespace knp::backends::multi_threaded_cpu
