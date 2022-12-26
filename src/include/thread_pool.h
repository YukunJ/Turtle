/**
 * @file thread_pool.h
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 26 2022
 *
 * This is a header file implementing the ThreadPool for maintaining and serving
 * the concurrent client requests
 */

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "utils.h"
#ifndef TURTLE_SERVER_THREAD_POOL_H
#define TURTLE_SERVER_THREAD_POOL_H

/* The minimum number of threads to exist in the threadpool */
#define MIN_NUM_THREADS_IN_POOL 2

namespace TURTLE_SERVER {

/**
 * This ThreadPool manages the thread resources and acts as the executor for
 * client requests upon submitting a task, it gives back a future to be waited
 * for
 * */
class ThreadPool {
 public:
  explicit ThreadPool(int size = std::thread::hardware_concurrency());

  ~ThreadPool();

  NON_COPYABLE(ThreadPool);

  template <typename F, typename... Args>
  decltype(auto) SubmitTask(F&& new_task, Args&&... args);

  void Exit();

 private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::atomic<bool> exit_{false};
};

template <typename F, typename... Args>
decltype(auto) ThreadPool::SubmitTask(F&& new_task, Args&&... args) {
  using return_type = std::invoke_result_t<F, Args...>;
  if (exit_) {
    throw std::runtime_error(
        "ThreadPool: SubmitTask() called while already exit_ being true");
  }
  auto packaged_new_task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(new_task), std::forward<Args>(args)...));
  auto fut = packaged_new_task->get_future();
  {
    // submit in form of std::function to the Thread Pool task queue
    std::unique_lock<std::mutex> lock(mtx_);
    tasks_.emplace([packaged_new_task]() { (*packaged_new_task)(); });
  }
  cv_.notify_one();
  return fut;
}
}  // namespace TURTLE_SERVER

#endif  // TURTLE_SERVER_THREAD_POOL_H
