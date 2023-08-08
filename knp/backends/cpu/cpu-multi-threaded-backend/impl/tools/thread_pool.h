/**
 * @file thread_pool.h
 * @brief Reusable thread pool class, a modified example from asio documentation.
 * @author Vartenkov A.
 * @date 27.07.2023
 */
#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/ts/executor.hpp>

/**
 * General namespace
 */
namespace knp::backends::multi_threaded_cpu
{

/**
 * @brief Thread pool context. A service class used for creating pool executors.
 * @note context lifetime should exceed lifetimes of its executors.
 * @note move and assignment are disabled.
 */
class ThreadPool
{
public:
    /**
     * @brief Constructor.
     * @param num_threads number of worker threads.
     */
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency()) : pool_(num_threads)
    {
        try
        {
            for (size_t thread_index = 0; thread_index < num_threads; ++thread_index)
            {
                boost::asio::post(
                    pool_,
                    [&]
                    {
                        std::unique_lock lock(mutex_);
                        while (usage_state_ != Usage::FINISHED)
                        {
                            if (!execute_next(lock)) condition_.wait(lock);
                        }
                    });
            }
        }
        catch (...)
        {
            stop();
            pool_.join();
        }
    }

    /**
     * @brief Destructor, blocking. Sends signal for threads to finish working, then joins all worker threads.
     */
    ~ThreadPool()
    {
        stop();
        pool_.join();
    }

    // Move and assignment are implicitly deleted because of mutex.


private:
    enum class Usage
    {
        FINISHED,
        READY,
        WORKING,
        STOPPING
    };

    struct Function
    {
        std::shared_ptr<size_t> work_count_;
        void (*execute_)(std::shared_ptr<Function> &p);
    };

    void do_work_started(const std::shared_ptr<size_t> &task_count) noexcept
    {
        ++(*task_count);
        usage_state_ = Usage::WORKING;
    }

    void do_work_finished(const std::shared_ptr<size_t> &task_count)
    {
        // Check if this was the final task.
        if (--(*task_count) == 0)
        {
            // use_count now should be equal to 1, we decrease it to 0 and signal all threads to stop.
            if (usage_state_ == Usage::STOPPING)
            {
                usage_state_ = Usage::FINISHED;
            }
            else
                usage_state_ = Usage::READY;
            condition_.notify_all();
        }
    }

    void execute(std::unique_lock<std::mutex> &lock, std::shared_ptr<Function> &work)
    {
        std::shared_ptr<size_t> work_count(std::move(work->work_count_));
        try
        {
            work->execute_(work);
            lock.lock();
            do_work_finished(work_count);
        }
        catch (...)
        {
            lock.lock();
            do_work_finished(work_count);
            throw;
        }
    }

    bool execute_next(std::unique_lock<std::mutex> &lock)
    {
        if (work_queue_.empty()) return false;
        auto task(work_queue_.front());
        work_queue_.pop();
        lock.unlock();
        execute(lock, task);
        return true;
    }

    void stop()
    {
        std::lock_guard lock_guard(mutex_);  // Not sure if we need it.
        if (usage_state_ == Usage::READY)
            usage_state_ = Usage::FINISHED;
        else
            usage_state_ = Usage::STOPPING;
        condition_.notify_all();
    }

    void post(std::shared_ptr<Function> task, const std::shared_ptr<size_t> &task_count)
    {
        std::lock_guard lock(mutex_);
        work_queue_.push(task);
        do_work_started(task_count);
        condition_.notify_one();
    }

private:
    friend class ThreadPoolExecutor;
    std::mutex mutex_;
    std::condition_variable condition_;
    Usage usage_state_ = Usage::READY;
    std::queue<std::shared_ptr<Function>> work_queue_;
    boost::asio::thread_pool pool_;
};


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
    explicit ThreadPoolExecutor(ThreadPool &context) : context_(context), task_count_(std::make_shared<size_t>(0)) {}

    [[nodiscard]] ThreadPool &context() { return context_; }

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
    struct Task : ThreadPool::Function
    {
        explicit Task(Func func, const std::shared_ptr<size_t> &task_count) : function(std::move(func))
        {
            work_count_ = task_count;
            execute_ = [](std::shared_ptr<ThreadPool::Function> &p)
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

    ThreadPool &context_;
    std::shared_ptr<size_t> task_count_;
};

}  // namespace knp::backends::multi_threaded_cpu
