project(elog_test)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(SRC_LIST  elog_test.cpp)
include_directories(../)
include_directories(../../be)
add_definitions("-Wall -g")
link_directories(/usr/local/lib)
link_directories(../lib)
link_directories(../../be/lib)
link_libraries(elog libbe.a pthread config)
add_executable(elog_test ${SRC_LIST})
