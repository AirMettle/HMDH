cmake_minimum_required(VERSION 3.14)

# Option to toggle auto-installing basic dependencies

include(${CMAKE_SOURCE_DIR}/config_options.cmake)

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

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

# Platform-specific configurations
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring for macOS.")
    set(OS "DARWIN")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS "LINUX")
    message(STATUS "Configuring for Ubuntu/Linux.")
else ()
    message(FATAL_ERROR "Unsupported OS")
endif ()



# Check if we should auto-install dependencies
if(AUTO_INSTALL_DEPENDENCIES)
    message(STATUS "Auto-installing basic dependencies...")
    include(${CMAKE_SOURCE_DIR}/install_tools.cmake)  # Adjust the path if necessary
    include(${CMAKE_SOURCE_DIR}/install_spdlog.cmake)

	# Install Crow
    message(STATUS "Installing Crow...")
    include(${CMAKE_SOURCE_DIR}/install_crow.cmake)

    # Install nlohmann/json TODO: FIX THE SCRIPT
    message(STATUS "Installing nlohmann/json...")
    include(${CMAKE_SOURCE_DIR}/install_nlohmann_json.cmake)
else()
    message(STATUS "Skipping auto-installation of dependencies. Please install manually.")
endif()

# Include install_gtest.cmake if ENABLE_TESTING is ON
if(ENABLE_TESTING)
    message(STATUS "Setting up Google Test for testing...")
    include(${CMAKE_SOURCE_DIR}/install_gtest.cmake)
else()
    message(STATUS "Testing disabled. Skipping Google Test installation.")
endif()
