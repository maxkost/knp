/**
 * @file thread_pool_executor.h
 * @brief Thread pool executor class.
 * @author Vartenkov A.
 * @date 08.08.2023
 */
#pragma once

#include <memory>
#include <utility>

#include "thread_pool_context.h"

/**
 * @brief Namespace for multi-threaded backend.
 */
namespace knp::backends::multi_threaded_cpu
{
/**
 * @brief Class for thread execution, this is the interface for thread pool.
 * @note Use boost::asio::post(executor, task) to queue more tasks. Use join() to wait for task execution.
 */
class ThreadPoolExecutor
{
public:
    /**
     * @brief Constructs pool executor.
     * @param context ThreadPool context. Its lifetime should be at least as long as for an object of this class.
     */
    explicit ThreadPoolExecutor(ThreadPoolContext &context)
        : context_(context), task_count_(std::make_shared<size_t>(0))
    {
    }

    [[nodiscard]] ThreadPoolContext &context() { return context_; }

    template <class Func, class Alloc>
    void post(Func function, const Alloc &allocator) const
    {
        auto new_task(std::allocate_shared<Task<Func>>(
            typename std::allocator_traits<Alloc>::template rebind_alloc<char>(allocator), std::move(function),
            task_count_));
        context_.post(new_task, task_count_);
    }

    /**
     * @brief Waits for all tasks to finish. Doesn't join the threads.
     */
    void join() const
    {
        std::unique_lock<std::mutex> lock(context_.mutex_);
        while (*task_count_ > 0)
            if (!context_.execute_next(lock)) context_.condition_.wait(lock);
    }

    /**
     * @brief Blocking destructor that waits for all tasks to be completed.
     */
    ~ThreadPoolExecutor() { join(); }

private:
    template <class Func>
    struct Task : ThreadPoolContext::Function
    {
        explicit Task(Func func, const std::shared_ptr<size_t> &task_count) : function(std::move(func))
        {
            work_count_ = task_count;
            execute_ = [](std::shared_ptr<ThreadPoolContext::Function> &p)
            {
                Func tmp(std::move(static_cast<Task *>(p.get())->function));
                p.reset();
                tmp();
            };
        }
        Func function;
    };

    // cppcheck-suppress unusedPrivateFunction
    void on_work_started()
    {
        std::lock_guard lock(context_.mutex_);
        context_.do_work_started(task_count_);
    }

    // cppcheck-suppress unusedPrivateFunction
    void on_work_finished()
    {
        std::lock_guard lock(context_.mutex_);
        context_.do_work_finished(task_count_);  // locks
    }


    /**
     * @brief Empty function needed to fulfil asio::post requirements
     */
    template <class Func, class Alloc>
    // cppcheck-suppress unusedPrivateFunction
    [[maybe_unused]] void dispatch(Func, const Alloc &) const
    {
    }

    /**
     * @brief Empty function needed to fulfil asio::post requirements.
     */
    template <class Func, class Alloc>
    // cppcheck-suppress unusedPrivateFunction
    [[maybe_unused]] void defer(Func, const Alloc &) const
    {
    }

    ThreadPoolContext &context_;
    std::shared_ptr<size_t> task_count_;
};

}  // namespace knp::backends::multi_threaded_cpu
