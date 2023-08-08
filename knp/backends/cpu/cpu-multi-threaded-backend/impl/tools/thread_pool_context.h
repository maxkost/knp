/**
 * @file thread_pool.h
 * @brief Context for reusable thread pool class, a modified example from asio documentation.
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
 * @brief Namespace for multi-threaded backend.
 */
namespace knp::backends::multi_threaded_cpu
{

/**
 * @brief Thread pool context. A service class used for creating pool executors.
 * @note context lifetime should exceed lifetimes of its executors.
 * @note move and assignment are disabled.
 */
class ThreadPoolContext
{
public:
    /**
     * @brief Constructor.
     * @param num_threads number of worker threads.
     */
    explicit ThreadPoolContext(size_t num_threads = std::thread::hardware_concurrency()) : pool_(num_threads)
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
    ~ThreadPoolContext()
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

    void post(const std::shared_ptr<Function> &task, const std::shared_ptr<size_t> &task_count)
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

}  // namespace knp::backends::multi_threaded_cpu
