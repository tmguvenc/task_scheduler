
project(scheduler)

file(GLOB src_files src/*.c*)
file(GLOB hdr_files include/*.h*)

include_directories(include)

set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE STRING "")

add_custom_target(
  format
  COMMAND
  ${CMAKE_CURRENT_SOURCE_DIR}/../tools/format.sh ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} SHARED ${src_files})

add_dependencies(${PROJECT_NAME} format)
