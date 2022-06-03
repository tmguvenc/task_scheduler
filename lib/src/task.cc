
#include "task.h"  // for Task

#include <sys/timerfd.h>
#include <unistd.h>

#include <ctime>
#include <numeric>
#include <random>

#include "common.h"

namespace ts {

Task::Task() {
  id_ = GetRandomValue(uint32_t{}, std::numeric_limits<uint32_t>::max());
  timer_fd_ = timerfd_create(CLOCK_REALTIME, 0);
}

Task::~Task() { close(timer_fd_); }

[[nodiscard]] uint32_t Task::GetUniqueId() const { return id_; }

[[nodiscard]] int Task::GetTimerFd() const { return timer_fd_; }

}  // namespace ts
