
#ifndef SCHEDULER_LIB_SCHEDULER_H_
#define SCHEDULER_LIB_SCHEDULER_H_

#include <atomic>         // for atomic_bool
#include <future>         // for future
#include <memory>         // for unique_ptr
#include <string>         // for string, basic_string, hash
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "common.h"  // for OpResult
#include "fd_container.h"

/** @brief forward-declare pollfd struct */
struct pollfd;

namespace ts {

/** @brief forward-declare Task interface */
class Task;

using CallbackFunc = std::function<void(const char* buf, const size_t len)>;
using TaskPtr = std::unique_ptr<Task>;

/**
 * @brief A class for managing task scheduling according to the tasks' periods.
 */
class TaskScheduler {
 public:
  /**
   * @brief Registers the task to the scheduler.
   * @param task Pointer to the task through its base
   * @return Error string in case of any error, nothing otherwise
   */
  [[nodiscard]] OpResult RegisterTask(TaskPtr task);

  /**
   * @brief Starts the tasks and fires their executors acc. to their periods.
   * @return Error string in case of any error, nothing otherwise
   */
  [[nodiscard]] OpResult Start();

  /**
   * @brief Starts the tasks
   * @return Error string in case of any error, nothing otherwise
   */
  [[nodiscard]] OpResult Stop();

 private:
  [[nodiscard]] OpResult ArmTimers();
  [[nodiscard]] std::vector<pollfd> CreateFdList();
  std::unordered_map<uint32_t, TaskPtr> task_map_;
  std::unordered_map<int, CallbackFunc> cb_map_;

  std::atomic_bool run_{false};
  std::future<void> thread_;
};

}  // namespace ts

#endif  // SCHEDULER_LIB_SCHEDULER_H_
