include_directories(include)

file(GLOB SOURCES "src/*.c")
add_library(vm ${SOURCES})

target_include_directories(vm
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS vm DESTINATION lib)
install(DIRECTORY include DESTINATION include)