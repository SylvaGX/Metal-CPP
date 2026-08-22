cmake_minimum_required(VERSION 3.16)
project(MetalKitSmokeTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(.. MetalCPP)

add_executable(MetalKitSmokeTest MetalKitSmokeTest.mm)
target_link_libraries(MetalKitSmokeTest PRIVATE MetalCPP)

target_include_directories(MetalKitSmokeTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
