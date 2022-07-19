include_directories(include)

file(GLOB SOURCES "src/*.cpp")
add_library(VM ${SOURCES})

target_include_directories(VM
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS VM DESTINATION lib)
install(DIRECTORY include DESTINATION include)