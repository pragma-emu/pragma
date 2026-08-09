cmake_minimum_required(VERSION 3.20)
project(Switch2Emulator_Project LANGUAGES CXX)

# Enforce the absolute newest C++ standards for advanced multi-threading hooks
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DYNARMIC_USE_GIT OFF)
set(DYNARMIC_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(externals/dynarmic)

add_executable(switch2_emu 
    src/main.cpp
    src/switch_cpu.cpp
    src/switch_cpu.h
)

target_include_directories(switch2_emu PRIVATE src)
target_link_libraries(switch2_emu PRIVATE dynarmic::dynarmic)
