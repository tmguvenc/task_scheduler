
#ifndef SCHEDULER_LIB_COMMON_H_
#define SCHEDULER_LIB_COMMON_H_

#include <chrono>
#include <optional>
#include <random>
#include <string>

namespace ts {

enum class EventType { MESSAGE_RECEIVED = 0, TIMEOUT, READY_TO_WRITE };

/** @brief Operation result that stores either nothing (in case of success) or
 * error string (in case of error) */
using OpResult = std::optional<std::string>;

template <typename Res = std::chrono::milliseconds>
static inline auto now() {
  return std::chrono::duration_cast<Res>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

static inline auto GetRandomSeed() {
  static std::random_device rd;
  return rd() ^
         (now<std::chrono::seconds>() + now<std::chrono::microseconds>());
}

template <typename T>
static inline auto GetRandomValue(const T min, const T max) {
  std::mt19937 gen(GetRandomSeed());
  std::uniform_int_distribution<T> distrib(min, max);
  return distrib(gen);
}

}  // namespace ts

#endif  // SCHEDULER_LIB_COMMON_H_
