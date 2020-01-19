# Copyright 2019 SwitchPy Team. All rights reserved.
# Licensed under the MIT license.
# Refer to the LICENSE file included.
#
# libnx CMake template for Nintendo Switch homebrew development.
project(screenuploader)

cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(options)
include(utils)

find_package(LIBNX REQUIRED)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(MbedTLS REQUIRED)
#find_package(LIBTWILI REQUIRED)
if (NOT LIBNX_FOUND)
    cmake_panic("Unable to detect libnx on this system.")
endif ()
if (NOT CURL_FOUND)
    cmake_panic("Unable to detect libcurl on this system.")
endif ()
if (NOT ZLIB_FOUND)
    cmake_panic("Unable to detect zlib on this system.")
endif ()
if (NOT MbedTLS_FOUND)
    cmake_panic("Unable to detect mbedtls on this system.")
endif ()
#if (NOT LIBTWILI_FOUND)
#    cmake_panic("Unable to detect libtwili on this system.")
#endif ()

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PORTLIBS}/include)

# Replace this with the name of your application
set(HOMEBREW_APP "screenuploader")

# Meta information about the app
set(APP_TITLE ${HOMEBREW_APP})
set(APP_AUTHOR "bakatrouble")
# set(APP_ICON "assets/icon.png")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_MICRO 1)
set(APP_VERSION
        "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}")

if (NOT DEFINED CMAKE_BUILD_TYPE_INIT)
    set(CMAKE_BUILD_TYPE_INIT Release)
endif ()

set(CMAKE_C_STANDARD 99)
if (USE_CPP_17)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    include(clang-utils)
endif ()

include(nx-utils)

cmake_info("Building ${APP_TITLE} version ${APP_VERSION}.")

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)

target_link_libraries(${HOMEBREW_APP}.elf ${CURL_LIBRARIES} ${MBEDTLS_LIBRARIES} ${ZLIB_LIBRARIES} switch::libnx)
set_target_properties(${HOMEBREW_APP}.elf PROPERTIES
        LINKER_LANGUAGE CXX # Replace this with C if you have C source files
        LINK_FLAGS "-specs=${LIBNX}/switch.specs -Wl,-no-as-needed -Wl,-Map,.map")

#build_switch_binaries(test.elf)
#build_switch_sysmodule(${HOMEBREW_APP}.elf)
build_switch_nsp(${HOMEBREW_APP}.elf)