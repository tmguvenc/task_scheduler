

project(test)
find_package(GTest REQUIRED)

set (tests
task_scheduler_test)

include_directories(include)
include_directories(${LIB_INCLUDE_DIR})

foreach(test ${tests})
  add_executable(${test} ${CMAKE_CURRENT_SOURCE_DIR}/src/${test}.cc)
  add_test(NAME ${test} COMMAND ${test})
  target_link_libraries(${test} scheduler)
  target_link_libraries(${test} ${GTEST_LIBRARIES})
  target_link_libraries(${test} pthread)
endforeach()
