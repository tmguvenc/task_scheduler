
project(scheduler)

file(GLOB src_files src/*.c*)
file(GLOB hdr_files include/*.h*)

include_directories(include)

set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE STRING "")

add_library(${PROJECT_NAME} SHARED ${src_files})

