
#ifndef SCHEDULER_LIB_TASK_H_
#define SCHEDULER_LIB_TASK_H_

#include <cstdint>
#include <string>

namespace ts {

/**
 * @brief An interface for the tasks
 */
class ITask {
 public:
  /**
   * @brief Default constructor.
   */
  ITask() = default;

  /**
   * @brief Default destructor.
   */
  virtual ~ITask() = default;

  /**
   * @brief Copy constructor.
   * @param other Instance to be copied.
   */
  ITask(const ITask& other) noexcept = delete;

  /**
   * @brief Copy assignment operator
   * @param other Instance to be copied.
   * @return Copied instance
   */
  ITask& operator=(const ITask& other) noexcept = delete;

  /**
   * @brief Move constructor.
   * @param other Instance to be moved.
   */
  ITask(ITask&& other) noexcept = delete;

  /**
   * @brief Move assignment operator
   * @param other Instance to be moved.
   * @return New instance
   */
  ITask& operator=(ITask&& other) noexcept = delete;

  /**
   * @brief Returns the name of the task.
   * @return name of the task
   */
  [[nodiscard]] virtual std::string GetName() const = 0;

  /**
   * @brief Returns the period of the task
   * @return period of the task in milliseconds.
   */
  [[nodiscard]] virtual uint64_t GetInterval() const = 0;

  /**
   * @brief Executes the task main functionality.
   */
  virtual void Execute() = 0;
};

}  // namespace uas::platform::task

#endif  // SCHEDULER_LIB_TASK_H_
