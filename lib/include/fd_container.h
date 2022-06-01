
#ifndef SCHEDULER_LIB_FD_CONTAINER_H_
#define SCHEDULER_LIB_FD_CONTAINER_H_

#include <algorithm>
#include <array>
#include <iostream>
#include <type_traits>
#include <unordered_set>

namespace ts {

template <size_t MaxLen>
struct Array {
  bool Add(const int fd) {
    if (elem_cnt == MaxLen - 1) return false;
    if (Contains(fd)) return false;

    arr[elem_cnt] = fd;
    ++elem_cnt;
    return true;
  }

  bool Contains(const int fd) const {
    return std::any_of(arr.begin(), arr.end(),
                       [s = fd](const int item) { return s == item; });
  }

 private:
  size_t elem_cnt{0};
  std::array<int, MaxLen> arr;
};

template <size_t MaxLen>
struct Set {
  bool Add(const int fd) {
    if (!Contains(fd) && set.size() < MaxLen) {
      set.insert(fd);
      return true;
    }
    return false;
  }

  bool Contains(const int fd) const { return set.find(fd) != set.end(); }

 private:
  std::unordered_set<int> set;
};

template <size_t MaxLen>
struct FdContainer {
  bool Add(const int fd) { return cont.Add(fd); }

  bool Contains(const int fd) const { return cont.Contains(fd); }

 private:
  static constexpr auto flg = MaxLen < 500;

  using C = std::conditional_t<flg, Array<MaxLen>, Set<MaxLen>>;
  C cont;
};

}  // namespace ts

#endif  // SCHEDULER_LIB_FD_CONTAINER_H_
