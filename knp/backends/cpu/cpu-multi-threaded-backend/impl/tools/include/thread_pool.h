/**
 * @file thread_pool.h
 * @brief Reusable thread pool class, a modified example from asio documentation, aggregates context and executor.
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
/**
 * @brief Thread pool class.
 */
class ThreadPool
{
public:
    /**
     * @brief Create a thread pool.
     * @param num_threads number of worker threads in the pool.
     */
    explicit ThreadPool(size_t num_threads)
        : context_(std::make_unique<ThreadPoolContext>(num_threads)), executor_(*context_)
    {
    }

    /**
     * @brief Add a task to the pool.
     * @tparam Func functional type.
     * @param task task to be run in the pool.
     * @note Not blocking.
     */
    template <class Func>
    void post(Func task)
    {
        boost::asio::post(executor_, task);
    }

    /**
     * @brief Wait until all threads stop processing.
     * @note Blocking, will wait indefinitely if at least one task never stops.
     */
    void join() { executor_.join(); }

private:
    // Don't change the order of declarations.
    std::unique_ptr<ThreadPoolContext> context_;
    ThreadPoolExecutor executor_;
};

}  // namespace knp::backends::multi_threaded_cpu
