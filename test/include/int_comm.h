
#ifndef SCHEDULER_TEST_INT_COMM_H_
#define SCHEDULER_TEST_INT_COMM_H_

#include <sys/eventfd.h>
#include <unistd.h>
#include "common.h"
#include <errno.h>
 #include <string.h>

struct IntComm {
  explicit IntComm() {fd_ = eventfd(0, 0);}
  ~IntComm() {close(fd_);}
  
  IntComm(const IntComm& other) noexcept = delete;
  IntComm& operator=(const IntComm& other) noexcept = delete;
  IntComm(IntComm&& other) noexcept = delete;
  IntComm& operator=(IntComm&& other) noexcept = delete;

  int GetFd() const { return fd_;}

  ts::OpResult Read(uint64_t* out_buf) const {
    if (!out_buf) {
      return "Invalid output buffer";
    }

    if (read(fd_, out_buf, sizeof(uint64_t)) == -1) {
      return "read failed: " + std::string(strerror(errno));
    }

    return {};
  }

  ts::OpResult Write(const uint64_t buf) const {
    if (write(fd_, &buf, sizeof(uint64_t)) == -1) {
      return "write failed: " + std::string(strerror(errno));
    }

    return {};
  }

private:
  int fd_;
};

#endif  // SCHEDULER_TEST_INT_COMM_H_
