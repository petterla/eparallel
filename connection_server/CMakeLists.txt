project(xylx)
set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions("-Wall -g")
include_directories(${PROJECT_SOURCE_DIR}/../be)
include_directories(${PROJECT_SOURCE_DIR}/proto)
include_directories(/usr/include/mysql)
include_directories(../)
include_directories(${PROJECT_SOURCE_DIR}/../efnfw)
include_directories(${PROJECT_SOURCE_DIR}/../elog)
link_directories(${PROJECT_SOURCE_DIR}/../be/lib)
link_directories(${PROJECT_SOURCE_DIR}/../efnfw/lib)
link_directories(${PROJECT_SOURCE_DIR}/../elog/lib)
link_directories(/usr/lib/mysql)
link_directories(/usr/lib64/mysql)
link_libraries(elog efnfw libprotobuf.a pthread config mysqlclient_r)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(xylx ${SRC_LIST})
