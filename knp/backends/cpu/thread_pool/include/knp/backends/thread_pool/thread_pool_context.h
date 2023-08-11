/**
 * @file thread_pool_context.h
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
 * @brief The ThreadPoolContext class is a service class used for creating pool executors.
 * @note Context lifetime should exceed lifetimes of its executors.\n 
 * Move and assignment are disabled.
 */
class ThreadPoolContext
{
public:
    /**
     * @brief Constructor.
     * @param num_threads number of worker threads.
     */
    explicit ThreadPoolContext(size_t num_threads = std::thread::hardware_concurrency());

    /**
     * @brief Blocking destructor.
     * @note The destructor sends signal for threads to finish working, then joins all worker threads.
     */
    ~ThreadPoolContext();

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
        // cppcheck-suppress unusedStructMember
        void (*execute_)(std::shared_ptr<Function> &p);
    };

    void do_work_started(const std::shared_ptr<size_t> &task_count) noexcept;

    void do_work_finished(const std::shared_ptr<size_t> &task_count);

    void execute(std::unique_lock<std::mutex> &lock, std::shared_ptr<Function> &work);

    bool execute_next(std::unique_lock<std::mutex> &lock);

    void stop();

    void post(const std::shared_ptr<Function> &task, const std::shared_ptr<size_t> &task_count);

private:
    friend class ThreadPoolExecutor;
    std::mutex mutex_;
    std::condition_variable condition_;
    Usage usage_state_ = Usage::READY;
    std::queue<std::shared_ptr<Function>> work_queue_;
    boost::asio::thread_pool pool_;
};

}  // namespace knp::backends::multi_threaded_cpu
