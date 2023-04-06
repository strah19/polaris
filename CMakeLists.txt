cmake_minimum_required(VERSION 3.10)

project(POLARIS VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_VM "Use Polaris's implemented virtual machine" OFF)

configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${PROJECT_SOURCE_DIR}/include/config.h)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

if(USE_VM)
    add_subdirectory(VM)
    list(APPEND EXTRA_LIBS VM)
endif()

add_executable(POLARIS ${SOURCES})

target_link_libraries(POLARIS PUBLIC ${EXTRA_LIBS})

target_include_directories(POLARIS PUBLIC "${PROJECT_BINARY_DIR}/include")

install(TARGETS POLARIS DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/include/config.h"
        DESTINATION include)

include(CTest)

add_test(NAME runs COMMAND POLARIS "../tests/basic.pol")