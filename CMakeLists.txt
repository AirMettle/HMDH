cmake_minimum_required(VERSION 3.14)

project(HMDH VERSION 1.0.0 LANGUAGES CXX)
add_compile_definitions(VERSION_NUMBER="${PROJECT_VERSION}")

# Set C++ standard/version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_SOURCE_DIR}/cmake/config_options.cmake)

if (APPLE)
    # For macOS, ensure that GCC-14 is used
    message(STATUS "Setting GCC 14 for macOS")
    # Find GCC and G++ version 14
    find_program(GCC_PATH gcc-14)
    find_program(GPP_PATH g++-14)
    # If GCC or G++ are not found, throw an error
    if (NOT GCC_PATH OR NOT GPP_PATH)
        message(FATAL_ERROR "GCC-14 and/or G++-14 not found! Ensure GCC is installed and available in your PATH.")
    endif ()
    # Set the compilers
    set(CMAKE_C_COMPILER "${GCC_PATH}")
    set(CMAKE_CXX_COMPILER "${GPP_PATH}")
    message(STATUS "Using GCC as C compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "Using G++ as C++ compiler: ${CMAKE_CXX_COMPILER}")
elseif (UNIX AND NOT APPLE)
    # For Ubuntu (assuming default GCC/G++ installations)
    message(STATUS "Setting default GCC/G++ for Ubuntu")
    # Find default GCC and G++
    find_program(GCC_PATH gcc)
    find_program(GPP_PATH g++)
    # If GCC or G++ are not found, throw an error
    if (NOT GCC_PATH OR NOT GPP_PATH)
        message(FATAL_ERROR "GCC and/or G++ not found! Ensure GCC is installed and available in your PATH.")
    endif ()
    # Set the compilers
    set(CMAKE_C_COMPILER "${GCC_PATH}")
    set(CMAKE_CXX_COMPILER "${GPP_PATH}")
    message(STATUS "Using GCC as C compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "Using G++ as C++ compiler: ${CMAKE_CXX_COMPILER}")
else ()
    message(FATAL_ERROR "Unsupported OS or platform.")
endif ()

# Detect architecture and define preprocessor directives
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    add_compile_definitions(x86_ARCH)
    set(ARCH "x86_64")
    message(STATUS "Detected architecture: x86_64. Defining x86_ARCH.")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_compile_definitions(ARM_ARCH)
    set(ARCH "aarch64")
    message(STATUS "Detected architecture: arm64. Defining ARM_ARCH.")
else ()
    message(STATUS "Detected architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}. No specific preprocessor definition added.")
endif ()

message(STATUS "ARCH: ${ARCH}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# Platform-specific configurations
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring for macOS.")
    set(OS "DARWIN")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS "LINUX")
    message(STATUS "Configuring for Ubuntu/Linux.")
    find_package(PkgConfig REQUIRED)
else ()
    message(FATAL_ERROR "Unsupported OS")
endif ()

# Add the project root directory to the include path
include_directories(${CMAKE_SOURCE_DIR} include/)

# Path to the local installation of dependencies
set(DEPENDENCY_DIR "$ENV{HOME}/.hmdh-deps")

# Include install_gtest.cmake if ENABLE_TESTING is ON
if(ENABLE_TESTING)
    message(STATUS "Setting up Google Test for testing...")
    include(${CMAKE_SOURCE_DIR}/cmake/install_gtest.cmake)
else()
    message(STATUS "Testing disabled. Skipping Google Test installation.")
endif()

# Add google test directory to CMake's search paths
set(GTEST_ROOT "${DEPENDENCY_DIR}/gtest")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS "LINUX")
    execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE ARCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # Read /etc/os-release to find the distribution ID
    file(READ "/etc/os-release" OS_RELEASE_CONTENT)
    string(REGEX MATCH "ID=[a-z]+" DISTRO_MATCH ${OS_RELEASE_CONTENT})

    if (DISTRO_MATCH)
        string(REPLACE "ID=" "" DISTRO ${DISTRO_MATCH})
    else ()
        message(WARNING "Failed to determine Linux distribution from /etc/os-release")
        set(DISTRO "") # Set DISTRO to a blank value
    endif ()
endif ()

message(STATUS "Detected distribution: ${DISTRO}")

if (OS STREQUAL "LINUX" AND ARCH STREQUAL "x86_64")
    set(SPDLOG_DIR "${DEPENDENCY_DIR}/spdlog/lib/cmake/spdlog")

elseif (OS STREQUAL "LINUX" AND ARCH STREQUAL "aarch64")
    if (DISTRO STREQUAL "ubuntu")
        set(SPDLOG_DIR "${DEPENDENCY_DIR}/spdlog/lib/cmake/spdlog")
    else ()
        set(SPDLOG_DIR "${DEPENDENCY_DIR}/spdlog/lib64/cmake/spdlog")
    endif ()

elseif (OS STREQUAL "DARWIN" AND ARCH STREQUAL "aarch64")
    set(SPDLOG_DIR ("${DEPENDENCY_DIR}/spdlog/lib/cmake/spdlog"))
else ()
    message(FATAL_ERROR "Unsupported combination of OS and architecture")
endif ()

message(STATUS "SPDLOG Directory: ${SPDLOG_DIR}")

find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED PATHS ${SPDLOG_DIR})

include_directories(SYSTEM
        include/
        lib/
        "${DEPENDENCY_DIR}/spdlog/include"

)

add_compile_definitions($<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:RELEASE_MODE>)


add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(UI)


# Enable testing
if (ENABLE_TESTING)
    message(STATUS "Testing enabled. Adding tests.")
    enable_testing()
    add_subdirectory(tests)
else ()
    message(STATUS "Testing disabled. Skipping tests.")
endif ()

