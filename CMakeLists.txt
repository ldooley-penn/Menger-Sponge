cmake_minimum_required(VERSION 3.10.0)
project(Menger-Sponge VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(external)

file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/*.h"
)

add_executable(Menger-Sponge ${SOURCE_FILES})

target_link_libraries(Menger-Sponge PRIVATE 
    glfw 
    glad 
    imgui 
    glm
)

target_include_directories(Menger-Sponge PRIVATE "src")

file(COPY "${CMAKE_SOURCE_DIR}/src/Shaders/" DESTINATION "${CMAKE_BINARY_DIR}/src/Shaders/")