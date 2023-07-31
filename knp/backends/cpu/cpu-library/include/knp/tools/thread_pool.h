/**
 * @file thread_pool.h
 * @brief Reusable thread pool class
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

namespace knp
{
class ThreadPool
{
public:
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

    void do_work_started(const std::shared_ptr<size_t> &task_count) noexcept  // done
    {
        ++(*task_count);
        usage_state_ = Usage::WORKING;
    }

    void do_work_finished(std::unique_lock<std::mutex> &lock, const std::shared_ptr<size_t> &task_count)  // done
    {
        lock.lock();
        // Check if this was the final task.
        if (--(*task_count) == 0)
        {
            // use_count now should be equal to 1, we decrease it to 0 and signal all threads to stop.
            if (usage_state_ == Usage::STOPPING)
            {
                usage_state_ = Usage::FINISHED;
                condition_.notify_all();
            }
            else
                usage_state_ = Usage::READY;
        }
    }

    void execute(std::unique_lock<std::mutex> &lock, std::shared_ptr<Function> &work)  // done
    {
        std::shared_ptr<size_t> work_count(std::move(work->work_count_));
        try
        {
            work->execute_(work);
            do_work_finished(lock, work_count);
        }
        catch (...)
        {
            do_work_finished(lock, work_count);
            throw;
        }
    }

    bool execute_next(std::unique_lock<std::mutex> &lock)  // done
    {
        if (work_queue_.empty()) return false;
        auto task(work_queue_.front());
        work_queue_.pop();
        lock.unlock();
        execute(lock, task);
        return true;
    }

    void stop()  // done
    {
        std::lock_guard lock_guard(mutex_);  // Not sure if we need it.
        usage_state_ = Usage::STOPPING;
        condition_.notify_all();
    }

    void post(std::shared_ptr<Function> task, const std::shared_ptr<size_t> &task_count)  // done
    {
        std::lock_guard lock(mutex_);
        work_queue_.push(task);
        do_work_started(task_count);
        condition_.notify_one();
    }

private:
    friend class Executor;
    std::mutex mutex_;
    std::condition_variable condition_;
    Usage usage_state_ = Usage::READY;
    std::queue<std::shared_ptr<Function>> work_queue_;
    boost::asio::thread_pool pool_;
};


class Executor
{
public:
    explicit Executor(ThreadPool &context) : context_(context), task_count_(std::make_shared<size_t>(0)) {}

    [[nodiscard]] ThreadPool &context() { return context_; }

    void on_work_started()
    {
        std::lock_guard lock(context_.mutex_);
        context_.do_work_started(task_count_);
    }

    void on_work_finished()
    {
        std::unique_lock lock(context_.mutex_);
        context_.do_work_finished(lock, task_count_);  // locks
        lock.unlock();
    }

    template <class Func, class Alloc>
    void post(Func function, const Alloc &allocator) const
    {
        auto new_task(std::allocate_shared<Task<Func>>(
            typename std::allocator_traits<Alloc>::template rebind_alloc<char>(allocator), std::move(function),
            task_count_));
        context_.post(new_task, task_count_);
    }

private:
    template <class Func>
    struct Task : ThreadPool::Function
    {
        explicit Task(Func function, const std::shared_ptr<size_t> &task_count) : function_(std::move(function))
        {
            work_count_ = task_count;
            execute_ = [](std::shared_ptr<ThreadPool::Function> &p)
            {
                Func tmp(std::move(static_cast<Task *>(p.get())->function));
                p.reset();
                tmp();
            };
        }
        Func function_;
    };

    ThreadPool &context_;
    std::shared_ptr<size_t> task_count_;
};


}  // namespace knp
