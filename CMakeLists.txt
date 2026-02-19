cmake_minimum_required(VERSION 3.10.0)
project(Menger-Sponge VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/*.h"
)

add_executable(Menger-Sponge ${SOURCE_FILES})

