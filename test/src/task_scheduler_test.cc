#include <gtest/gtest.h>

#include "common.h"
#include "task.h"
#include "task_scheduler.h"

#include <sys/eventfd.h>
#include <unistd.h>

class TimedTask : public ts::Task {
public:
  explicit TimedTask() { fd_ = eventfd(0, 0); }

  ~TimedTask() { close(fd_); }

  [[nodiscard]] uint64_t GetInterval() const override { return 50; }

  [[nodiscard]] int GetIoFd() const { return fd_; }

  void Handle(const ts::EventType evt_type, const char *data,
              const size_t len) override {
    switch (evt_type) {
    case ts::EventType::MESSAGE_RECEIVED:

      break;
    case ts::EventType::TIMEOUT:

      break;
    case ts::EventType::READY_TO_WRITE:

      break;

    default:
      break;
    }
  }

private:
  int fd_;
};

TEST(timed_task_test, timeout) {
  ts::TaskScheduler ts;
  auto ret = ts.RegisterTask(std::make_unique<TimedTask>());
  ASSERT_FALSE(ret.has_value());

  ret = ts.Start();
  ASSERT_FALSE(ret.has_value());

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  ret = ts.Stop();
  ASSERT_FALSE(ret.has_value());
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}