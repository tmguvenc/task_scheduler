
#ifndef SCHEDULER_LIB_COMMON_H_
#define SCHEDULER_LIB_COMMON_H_

#include <optional>
#include <string>

namespace ts {

/** @brief Operation result that stores either nothing (in case of success) or
 * error string (in case of error) */
using OpResult = std::optional<std::string>;

}  // namespace ts

#endif  // SCHEDULER_LIB_COMMON_H_
