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

    void do_work_started(const std::shared_ptr<size_t> &task_count) noexcept  // done
    {
        ++(*task_count);
        usage_state_ = Usage::WORKING;
    }

    void do_work_finished(const std::shared_ptr<size_t> &task_count)  // done
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

    void execute(std::unique_lock<std::mutex> &lock, std::shared_ptr<Function> &work)  // done
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
        if (usage_state_ == Usage::READY)
            usage_state_ = Usage::FINISHED;
        else
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
    friend class ThreadPoolExecutor;
    std::mutex mutex_;
    std::condition_variable condition_;
    Usage usage_state_ = Usage::READY;
    std::queue<std::shared_ptr<Function>> work_queue_;
    boost::asio::thread_pool pool_;
};


class ThreadPoolExecutor
{
public:
    explicit ThreadPoolExecutor(ThreadPool &context) : context_(context), task_count_(std::make_shared<size_t>(0)) {}

    [[nodiscard]] ThreadPool &context() { return context_; }

    void on_work_started()
    {
        std::lock_guard lock(context_.mutex_);
        context_.do_work_started(task_count_);
    }

    void on_work_finished()
    {
        std::lock_guard lock(context_.mutex_);
        context_.do_work_finished(task_count_);  // locks
    }

    template <class Func, class Alloc>
    void post(Func function, const Alloc &allocator) const
    {
        auto new_task(std::allocate_shared<Task<Func>>(
            typename std::allocator_traits<Alloc>::template rebind_alloc<char>(allocator), std::move(function),
            task_count_));
        context_.post(new_task, task_count_);
    }

    void join() const
    {
        std::unique_lock<std::mutex> lock(context_.mutex_);
        while (*task_count_ > 0)
            if (!context_.execute_next(lock)) context_.condition_.wait(lock);
    }

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

    template <class Func, class Alloc>
    // cppcheck-suppress unusedPrivateFunction
    [[maybe_unused]] void dispatch(Func, const Alloc &) const
    {
    }

    template <class Func, class Alloc>
    // cppcheck-suppress unusedPrivateFunction
    [[maybe_unused]] void defer(Func, const Alloc &) const
    {
    }

    ThreadPool &context_;
    std::shared_ptr<size_t> task_count_;
};

}  // namespace knp
