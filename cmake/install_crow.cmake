# Define base directory for dependencies
set(DEPENDENCY_DIR "$ENV{HOME}/.hmdh-deps")

# Define the installation directory for Google Test (gtest)
set(CROW_DIR ${DEPENDENCY_DIR}/crow)

# Ensure the dependencies directory exists
if(NOT EXISTS ${CROW_DIR})
    message(STATUS "Creating directory ${CROW_DIR} for Crow installation...")
    file(MAKE_DIRECTORY ${CROW_DIR})
else()
    message(STATUS "Directory ${CROW_DIR} already exists.")
endif()

# Check if the Crow directory is non-empty
file(GLOB CROW_CONTENTS "${CROW_DIR}/*")
if(CROW_CONTENTS)
    message(STATUS "Crow is already installed. Skipping installation")
else()
    message(STATUS "Crow directory is empty or doesn't exist. Proceeding with installation")

    # Set up the ExternalProject for Google Test
    include(ExternalProject)

    # Determine the number of available processors in a cross-platform way
    include(ProcessorCount)
    ProcessorCount(NPROC)
    if(NOT NPROC)
        set(NPROC 1)  # Default to 1 if ProcessorCount fails
    endif()

    # Add the ExternalProject to download and install Google Test
    ExternalProject_Add(crow
            PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/crow_build
            GIT_REPOSITORY "https://github.com/CrowCpp/Crow.git"
            GIT_TAG "master"
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CROW_DIR}
            -DCMAKE_BUILD_TYPE=Release
            BUILD_COMMAND make -j${NPROC}
            INSTALL_COMMAND make install
    )

    # Custom target to clean the build directory after installation
    add_custom_target(clean_crow_build ALL
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/crow_build
            COMMENT "Cleaning up crow_build directory after installation"
    )

    # Ensure the clean target runs after crow is built
    add_dependencies(clean_crow_build crow)
endif()
