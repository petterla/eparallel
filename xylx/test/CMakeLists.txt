project(xylxtest)
set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions("-Wall -g")
link_libraries(libprotobuf.a)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(xylxtest ${SRC_LIST})
