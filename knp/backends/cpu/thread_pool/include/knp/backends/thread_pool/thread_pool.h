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
namespace knp::backends::tools
{
/**
 * @brief The ThreadPool class is a definition of thread pool.
 */
class ThreadPool
{
public:
    /**
     * @brief Create thread pool.
     * @param num_threads number of worker threads in the pool.
     */
    explicit ThreadPool(size_t num_threads)
        : context_(std::make_unique<ThreadPoolContext>(num_threads)), executor_(*context_)
    {
    }

    /**
     * @brief Add task to pool.
     * @tparam Func function type.
     * @tparam Args function arguments.
     * @param func task to run in the pool.
     * @param args function arguments (if required, use `std::ref`).
     * @note Non-blocking method.
     */
    template <class Func, typename... Args>
    void post(Func func, Args... args)
    {
        boost::asio::post(executor_, std::bind(func, args...));
    }

    /**
     * @brief Wait until all threads stop processing.
     * @note Blocking method that waits indefinitely if at least one task never stops.
     */
    void join() { executor_.join(); }

private:
    // Don't change the order of declarations.
    std::unique_ptr<ThreadPoolContext> context_;
    ThreadPoolExecutor executor_;
};

}  // namespace knp::backends::tools
