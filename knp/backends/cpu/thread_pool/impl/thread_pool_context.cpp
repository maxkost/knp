/**
 * @file thread_pool_context.cpp
 * @brief Thread pool executor class implementation.
 * @author Vartenkov A.
 * @date 08.08.2023
 */
#include <knp/backends/thread_pool/thread_pool_context.h>

/**
 * @brief Namespace for backend tools.
 */
namespace knp::backends::tools
{
ThreadPoolContext::ThreadPoolContext(size_t num_threads) : pool_(num_threads)
{
    try
    {
        for (size_t thread_index = 0; thread_index < num_threads; ++thread_index)
        {
            boost::asio::post(
                pool_,
                [this]
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


ThreadPoolContext::~ThreadPoolContext()
{
    stop();
    pool_.join();
}


void ThreadPoolContext::do_work_started(const std::shared_ptr<size_t> &task_count) noexcept
{
    ++(*task_count);
    usage_state_ = Usage::WORKING;
}


void ThreadPoolContext::do_work_finished(const std::shared_ptr<size_t> &task_count)
{
    // Check if this was the final task.
    if (--(*task_count) == 0)
    {
        usage_state_ = usage_state_ == Usage::STOPPING ? Usage::FINISHED : Usage::READY;
        condition_.notify_all();
    }
}


void ThreadPoolContext::execute(std::unique_lock<std::mutex> &lock, std::shared_ptr<Function> &work)
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


bool ThreadPoolContext::execute_next(std::unique_lock<std::mutex> &lock)
{
    if (work_queue_.empty()) return false;
    auto task(work_queue_.front());
    work_queue_.pop();
    lock.unlock();
    execute(lock, task);
    return true;
}


void ThreadPoolContext::stop()
{
    std::lock_guard lock_guard(mutex_);
    usage_state_ = usage_state_ == Usage::READY ? Usage::FINISHED : Usage::STOPPING;
    condition_.notify_all();
}


void ThreadPoolContext::post(const std::shared_ptr<Function> &task, const std::shared_ptr<size_t> &task_count)
{
    std::lock_guard lock(mutex_);
    work_queue_.push(task);
    do_work_started(task_count);
    condition_.notify_one();
}
}  // namespace knp::backends::tools
