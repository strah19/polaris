cmake_minimum_required(VERSION 3.10)

project(POLARIS VERSION 1.0)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O -O3")

configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${PROJECT_SOURCE_DIR}/include/config.h)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(POLARIS ${SOURCES})

add_subdirectory(VM)
list(APPEND EXTRA_LIBS VM)

target_link_libraries(POLARIS PUBLIC ${EXTRA_LIBS})

target_include_directories(POLARIS PUBLIC "${PROJECT_BINARY_DIR}/include")

install(TARGETS POLARIS DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/include/config.h"
        DESTINATION include)

enable_testing()
include(CTest)

add_test(NAME Assignment COMMAND POLARIS "../tests/assignment.pol")