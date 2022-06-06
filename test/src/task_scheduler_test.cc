#include <gtest/gtest.h>

#include "common.h"
#include "task.h"
#include "task_scheduler.h"

#include "int_comm.h"

class TimedTask : public ts::Task {
public:
  explicit TimedTask(int comm_fd) : comm_fd_(comm_fd) { fd_ = eventfd(0, 0); }
  ~TimedTask() { close(fd_); }

  TimedTask(const TimedTask& other) noexcept = delete;
  TimedTask& operator=(const TimedTask& other) noexcept = delete;
  TimedTask(TimedTask&& other) noexcept = delete;
  TimedTask& operator=(TimedTask&& other) noexcept = delete;

  [[nodiscard]] uint64_t GetInterval() const override { return 500; }

  [[nodiscard]] int GetIoFd() const { return fd_; }

  void Handle(const ts::EventType evt_type, const char *data,
              const size_t len) override {
    if (write(comm_fd_, (uint64_t*)(&evt_type), sizeof(uint64_t)) == -1) {
      fprintf(stderr, "write failed: %s", strerror(errno));
    }
  }

private:
  int fd_;
  int comm_fd_;
};

class IoTask : public ts::Task {
public:
  explicit IoTask(int io_fd, int comm_fd) : io_fd_(io_fd), comm_fd_(comm_fd) { }
  ~IoTask() = default;

  IoTask(const IoTask& other) noexcept = delete;
  IoTask& operator=(const IoTask& other) noexcept = delete;
  IoTask(IoTask&& other) noexcept = delete;
  IoTask& operator=(IoTask&& other) noexcept = delete;

  [[nodiscard]] uint64_t GetInterval() const override { return 500; }

  [[nodiscard]] int GetIoFd() const { return io_fd_; }

  void Handle(const ts::EventType evt_type, const char *data,
              const size_t len) override {
    if (write(comm_fd_, (uint64_t*)(&evt_type), sizeof(uint64_t)) == -1) {
      fprintf(stderr, "write failed: %s", strerror(errno));
    }
  }

private:
  int io_fd_;
  int comm_fd_;
};

TEST(timed_task_test, timeout) {
  ts::TaskScheduler ts;
  IntComm int_comm;
  ASSERT_NE(int_comm.GetFd(), -1);

  auto ret = ts.RegisterTask(std::make_unique<TimedTask>(int_comm.GetFd()));
  ASSERT_FALSE(ret.has_value());

  ret = ts.Start();
  ASSERT_FALSE(ret.has_value());

  uint64_t u;

  const auto res = int_comm.Read(&u);
  ASSERT_FALSE(res.has_value());

  ASSERT_EQ(static_cast<ts::EventType>(u), ts::EventType::TIMEOUT);

  ret = ts.Stop();
  ASSERT_FALSE(ret.has_value());
}

TEST(timed_task_test, io) {
  ts::TaskScheduler ts;
  IntComm int_comm;
  ASSERT_NE(int_comm.GetFd(), -1);
  
  IntComm int_io;
  ASSERT_NE(int_io.GetFd(), -1);

  auto ret = ts.RegisterTask(std::make_unique<IoTask>(int_io.GetFd(), int_comm.GetFd()));
  ASSERT_FALSE(ret.has_value());

  ret = ts.Start();
  ASSERT_FALSE(ret.has_value());

  ret = int_io.Write(12);
  ASSERT_FALSE(ret.has_value());

  uint64_t u;

  ret = int_comm.Read(&u);
  ASSERT_FALSE(ret.has_value());

  ASSERT_EQ(static_cast<ts::EventType>(u), ts::EventType::MESSAGE_RECEIVED);

  ret = int_comm.Read(&u);
  ASSERT_FALSE(ret.has_value());

  ASSERT_EQ(static_cast<ts::EventType>(u), ts::EventType::TIMEOUT);

  ret = ts.Stop();
  ASSERT_FALSE(ret.has_value());
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}