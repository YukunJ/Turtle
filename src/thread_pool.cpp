/**
 * @file thread_pool.cpp
 * @author Yukun J
 * @expectation this header file should be compatible to compile in C++
 * program on Linux
 * @init_date Dec 26 2022
 *
 * This is an implementation file implementing the ThreadPool for maintaining
 * and serving the concurrent client requests
 */

#include "thread_pool.h"

namespace TURTLE_SERVER {

ThreadPool::ThreadPool(int size) {
  /* std::thread::hardware_concurrency() might return 0 if sys info not
   * available */
  size = std::max(size, MIN_NUM_THREADS_IN_POOL);
  for (auto i = 0; i < size; i++) {
    threads_.emplace_back([this]() {
      while (true) {
        std::function<void()> next_task;
        {
          std::unique_lock<std::mutex> lock(mtx_);
          cv_.wait(lock, [this]() { return exit_ || !tasks_.empty(); });
          if (exit_ && tasks_.empty()) {
            return;  // thread life ends
          }
          next_task = tasks_.front();
          tasks_.pop();
        }
        next_task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  Exit();
  for (auto& worker : threads_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void ThreadPool::Exit() {
  exit_ = true;
  cv_.notify_all();
}
}  // namespace TURTLE_SERVER