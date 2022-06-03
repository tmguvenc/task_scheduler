
#ifndef SCHEDULER_LIB_TASK_H_
#define SCHEDULER_LIB_TASK_H_

#include <cstdint>
#include <string>

#include "common.h"

namespace ts {

/**
 * @brief An abstrct base class for the tasks
 */
class Task {
 public:
  /**
   * @brief Default constructor.
   */
  Task();

  /**
   * @brief Default destructor.
   */
  virtual ~Task();

  /**
   * @brief Copy constructor.
   * @param other Instance to be copied.
   */
  Task(const Task& other) noexcept = delete;

  /**
   * @brief Copy assignment operator
   * @param other Instance to be copied.
   * @return Copied instance
   */
  Task& operator=(const Task& other) noexcept = delete;

  /**
   * @brief Move constructor.
   * @param other Instance to be moved.
   */
  Task(Task&& other) noexcept = delete;

  /**
   * @brief Move assignment operator
   * @param other Instance to be moved.
   * @return New instance
   */
  Task& operator=(Task&& other) noexcept = delete;

  /**
   * @brief Returns the unique id of the task.
   * @return unique id of the task
   */
  [[nodiscard]] uint32_t GetUniqueId() const;

  /**
   * @brief Returns the timer fd of the task.
   * @return timer fd of the task
   */
  [[nodiscard]] int GetTimerFd() const;

  /**
   * @brief Returns the period of the task
   * @return period of the task in milliseconds.
   */
  [[nodiscard]] virtual uint64_t GetInterval() const = 0;

  /**
   * @brief Returns the io fd of the task
   * @return IO file descriptor
   */
  [[nodiscard]] virtual int GetIoFd() const = 0;

  virtual void Handle(const EventType evt_type, const char* data,
                      const size_t len) = 0;

 private:
  int timer_fd_;
  uint32_t id_;
};

}  // namespace ts

#endif  // SCHEDULER_LIB_TASK_H_
