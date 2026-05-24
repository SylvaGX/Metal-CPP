#--------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# Additional copyright for AppKit contributions:
#
# Copyright 2026 SylvaGX.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)
project(MetalCPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENV{OBJC_DEBUG_MISSING_POOLS} YES)

option(METALCPP_ENABLE_QUARTZCORE "Add QuartzCore include path and link the QuartzCore framework" OFF)

add_library(MetalCPP INTERFACE
    AppKit
    Foundation
    Metal
    MetalFX
)

if(METALCPP_ENABLE_QUARTZCORE)
    add_library(MetalCPP INTERFACE
        QuartzCore
    )
endif()

target_include_directories(MetalCPP INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/AppKit"
    "${CMAKE_CURRENT_SOURCE_DIR}/Foundation"
    "${CMAKE_CURRENT_SOURCE_DIR}/Metal"
    "${CMAKE_CURRENT_SOURCE_DIR}/MetalFX"
)

if(METALCPP_ENABLE_QUARTZCORE)
    target_include_directories(MetalCPP INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/QuartzCore"
    )
endif()

find_library(METAL_LIBRARY Metal REQUIRED)
find_library(METALKIT_LIBRARY MetalKit REQUIRED)
find_library(APPKIT_LIBRARY AppKit REQUIRED)
find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
# Add IOKit later
# find_library(IOKIT_LIBRARY IOKit REQUIRED)

if(METALCPP_ENABLE_QUARTZCORE)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)
endif()

target_link_libraries(MetalCPP INTERFACE
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${APPKIT_LIBRARY}
    ${FOUNDATION_LIBRARY}
)

if(METALCPP_ENABLE_QUARTZCORE)
    target_link_libraries(MetalCPP INTERFACE {QUARTZCORE_LIBRARY})
endif()
