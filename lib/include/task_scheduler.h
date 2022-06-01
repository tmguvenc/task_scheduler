
#ifndef SCHEDULER_LIB_SCHEDULER_H_
#define SCHEDULER_LIB_SCHEDULER_H_

#include <atomic>         // for atomic_bool
#include <future>         // for future
#include <string>         // for string, basic_string, hash
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "common.h"  // for OpResult
#include "fd_container.h"

/** @brief forward-declare pollfd struct */
struct pollfd;

namespace ts {

/** @brief forward-declare ITask interface */
class ITask;

/**
 * @brief A class for managing task scheduling according to the tasks' periods.
 */
class TaskScheduler {
  /**
   * @brief A wrapper struct for containing task itself and its file descriptor.
   */
  struct Timer {
    /** @brief pointer to the task */
    ITask* task{nullptr};

    /** @brief file descriptor to monitor the timeout event via \b poll */
    int fd{-1};
  };

 public:
  /**
   * @brief Registers the task to the scheduler.
   * @param task Pointer to the task through its base
   * @return Error string in case of any error, nothing otherwise
   */
  [[nodiscard]] OpResult RegisterTask(ITask* task);

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
  std::vector<pollfd> CreateFdList();

  std::unordered_map<std::string, Timer> task_map_{};
  std::unordered_map<int, ITask*> fd_map_{};
  FdContainer<100> timer_fds_{};
  FdContainer<100> io_fds_{};
  std::atomic_bool run_{false};
  std::future<void> thread_;
};

}  // namespace ts

#endif  // SCHEDULER_LIB_SCHEDULER_H_
