# Define base directory for dependencies
set(DEPENDENCY_DIR "$ENV{HOME}/.hmdh-deps")

# Define the installation directory for Google Test (gtest)
set(GTEST_DIR ${DEPENDENCY_DIR}/gtest)

# Ensure the dependencies directory exists
if(NOT EXISTS ${GTEST_DIR})
    message(STATUS "Creating directory ${GTEST_DIR} for Google Test installation...")
    file(MAKE_DIRECTORY ${GTEST_DIR})
else()
    message(STATUS "Directory ${GTEST_DIR} already exists.")
endif()

# Check if the Google Test directory is non-empty
file(GLOB GTEST_CONTENTS "${GTEST_DIR}/*")
if(GTEST_CONTENTS)
    message(STATUS "Google Test is already installed. Skipping installation")
else()
    message(STATUS "Google Test directory is empty or doesn't exist. Proceeding with installation")

    # Set up the ExternalProject for Google Test
    include(ExternalProject)

    # Determine the number of available processors in a cross-platform way
    include(ProcessorCount)
    ProcessorCount(NPROC)
    if(NOT NPROC)
        set(NPROC 1)  # Default to 1 if ProcessorCount fails
    endif()

    # Add the ExternalProject to download and install Google Test
    ExternalProject_Add(gtest
            PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/gtest_build
            GIT_REPOSITORY "https://github.com/google/googletest.git"
            GIT_TAG "release-1.12.1"
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${GTEST_DIR}
            -DCMAKE_BUILD_TYPE=Release
            BUILD_COMMAND make -j${NPROC}
            INSTALL_COMMAND make install
    )

    # Custom target to clean the build directory after installation
    add_custom_target(clean_gtest_build ALL
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/gtest_build
            COMMENT "Cleaning up gtest_build directory after installation"
    )

    # Ensure the clean target runs after GTest is built
    add_dependencies(clean_gtest_build gtest)
endif()
