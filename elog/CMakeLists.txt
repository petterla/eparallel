project(elog)
set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions("-Wall -g")
include_directories(${PROJECT_SOURCE_DIR}/../be)
link_directories(../../be/lib)
set(LIBRARY_OUTPUT_PATH lib)
link_libraries(be pthread config)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_library(elog STATIC ${SRC_LIST})
SET_TARGET_PROPERTIES(elog PROPERTIES LINKER_LANGUAGE C)
