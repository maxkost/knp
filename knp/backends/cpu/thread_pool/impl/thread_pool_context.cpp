/**
 * @file thread_pool_context.cpp
 * @brief Thread pool executor class implementation.
 * @kaspersky_support Vartenkov A.
 * @date 08.08.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <knp/backends/thread_pool/thread_pool_context.h>

/**
 * @brief Namespace for CPU backend executors.
 */
namespace knp::backends::cpu_executors
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
}  // namespace knp::backends::cpu_executors
