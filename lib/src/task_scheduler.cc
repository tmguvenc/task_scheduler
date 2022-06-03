
#include "task_scheduler.h"

#include <bits/types/struct_itimerspec.h>  // for itimerspec
#include <cxxabi.h>                        // for __forced_unwind
#include <poll.h>                          // for pollfd, poll, POLLIN
#include <sys/timerfd.h>                   // for timerfd_create, timerfd_se...
#include <unistd.h>                        // for close, read

#include <algorithm>     // for for_each, max
#include <cerrno>        // for errno
#include <cstdint>       // for int64_t, uint64_t
#include <cstring>       // for strerror
#include <ctime>         // for timespec, clock_gettime
#include <optional>      // for optional
#include <system_error>  // for system_error
#include <utility>       // for pair

#include "task.h"  // for ITask

static constexpr int64_t kSecToMSec = 1e3;
static constexpr int64_t kMSecToNSec = 1e6;

static inline auto ConvertToTimespec(const int64_t interval_ms) {
  return timespec{
      .tv_sec = interval_ms / kSecToMSec,
      .tv_nsec = (interval_ms - (interval_ms / kSecToMSec) * kSecToMSec) *
                 kMSecToNSec};
}

namespace ts {

[[nodiscard]] static inline OpResult StopTimer(int fd) {
  itimerspec new_value = {0};

  if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, nullptr) == -1) {
    return "stop timer failed: " + std::string(std::strerror(errno));
  }

  return {};
}

[[nodiscard]] static inline OpResult StartTimer(int fd,
                                                const int64_t interval_ms) {
  timespec now{};
  if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
    return "clock_gettime failed: " + std::string(std::strerror(errno));
  }

  const auto ti = ConvertToTimespec(interval_ms);

  // clang-format off
    itimerspec new_value {
      // expiration interval
      .it_interval = {
        .tv_sec = ti.tv_sec,
        .tv_nsec = ti.tv_nsec
      },

      // initial expiration time
      .it_value = {
        .tv_sec = now.tv_sec + ti.tv_sec,
        .tv_nsec = now.tv_nsec + ti.tv_nsec
      }
    };
  // clang-format on

  if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, nullptr) == -1) {
    return "timerfd_settime failed: " + std::string(std::strerror(errno));
  }

  return {};
}

[[nodiscard]] static inline OpResult RestartTimer(int fd,
                                                  const int64_t interval_ms) {
  if (const auto ret = StopTimer(fd); ret.has_value()) {
    return *ret;
  }

  if (const auto ret = StartTimer(fd, interval_ms); ret.has_value()) {
    return *ret;
  }

  return {};
}

[[nodiscard]] OpResult TaskScheduler::RegisterTask(TaskPtr task) {
  if (!task) {
    return "Task cannot be nullptr";
  }

  const auto unique_id = task->GetUniqueId();
  const auto timer_id = task->GetTimerFd();
  const auto io_id = task->GetIoFd();

  if (const auto it = task_map_.find(unique_id); it != task_map_.end()) {
    return "Task is already registered :" + std::to_string(unique_id);
  }

  task_map_[unique_id] = std::move(task);

  cb_map_[timer_id] = [&t = task_map_[unique_id]](const char* /*buf*/,
                                                  const size_t /*len*/) {
    t->Handle(EventType::TIMEOUT, nullptr, 0);
  };

  cb_map_[io_id] = [&t = task_map_[unique_id]](const char* buf,
                                               const size_t len) {
    t->Handle(EventType::MESSAGE_RECEIVED, buf, len);
  };

  return {};
}

[[nodiscard]] OpResult TaskScheduler::Start() {
  thread_ = std::async([&]() {
    auto fd_list = CreateFdList();
    if (fd_list.empty()) {
      return;
    }

    if (const auto ret = ArmTimers(); ret.has_value()) {
      return;
    }

    run_.store(true);
    uint64_t exp{};

    std::array<char, 1024> buf{};

    while (run_.load()) {
      const auto ret = poll(fd_list.data(), fd_list.size(), -1);
      if (ret < 0) {
        continue;
      }

      std::for_each(fd_list.begin(), fd_list.end(), [&](const pollfd& pfd) {
        if (pfd.revents & POLLIN) {
          if (const auto it = cb_map_.find(pfd.fd); it != cb_map_.end()) {
            const auto len = read(pfd.fd, buf.data(), buf.size());
            it->second(buf.data(), len);
          }
        }
      });
    }
  });

  return {};
}

[[nodiscard]] OpResult TaskScheduler::Stop() {
  if (!run_.load()) {
    return "Task Scheduler not started yet!";
  }

  run_.store(false);
  if (!thread_.valid()) {
    return "Task Scheduler thread is not valid!";
  }

  thread_.get();
  return {};
}

[[nodiscard]] OpResult TaskScheduler::ArmTimers() {
  for (const auto& t : task_map_) {
    if (const auto ret = StartTimer(t.first, t.second->GetInterval());
        ret.has_value()) {
      return *ret;
    }
  }
  return {};
}

[[nodiscard]] std::vector<pollfd> TaskScheduler::CreateFdList() {
  std::vector<pollfd> fd_list;
  fd_list.reserve(task_map_.size());

  std::for_each(task_map_.begin(), task_map_.end(), [&](const auto& p) {
    fd_list.push_back({.fd = p.second->GetIoFd(), .events = POLLIN});
    fd_list.push_back({.fd = p.second->GetTimerFd(), .events = POLLIN});
  });

  return fd_list;
}
}  // namespace ts
