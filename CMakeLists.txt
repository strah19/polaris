cmake_minimum_required(VERSION 3.10)

project(POLARIS VERSION 1.0)

configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${PROJECT_SOURCE_DIR}/include/config.h)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(POLARIS ${SOURCES})

target_link_libraries(POLARIS PUBLIC)

target_include_directories(POLARIS PUBLIC "${PROJECT_BINARY_DIR}/include")

install(TARGETS POLARIS DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/include/config.h"
        DESTINATION include)

include(CTest)

add_test(NAME runs COMMAND POLARIS "../tests/basic.pol")