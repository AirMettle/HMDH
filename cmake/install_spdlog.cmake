# Define base directory for dependencies
set(DEPENDENCY_DIR "$ENV{HOME}/.hmdh-deps")

# Ensure the dependencies directory exists, create if not
if(NOT EXISTS ${DEPENDENCY_DIR})
    message(STATUS "Creating directory ${DEPENDENCY_DIR} for dependencies...")
    file(MAKE_DIRECTORY ${DEPENDENCY_DIR})
else()
    message(STATUS "Directory ${DEPENDENCY_DIR} already exists.")
endif()

# Define the installation directory for spdlog
set(SPDLOG_DIR ${DEPENDENCY_DIR}/spdlog)

# Ensure the install directory exists, create if not
if(NOT EXISTS ${SPDLOG_DIR})
    message(STATUS "Creating directory ${SPDLOG_DIR} for spdlog installation...")
    file(MAKE_DIRECTORY ${SPDLOG_DIR})
else()
    message(STATUS "Directory ${SPDLOG_DIR} already exists.")
endif()

# Check if the spdlog directory is non-empty
file(GLOB SPDLOG_CONTENTS "${SPDLOG_DIR}/*")
if(SPDLOG_CONTENTS)
    message(STATUS "spdlog is already installed. Skipping installation.")
else()
    message(STATUS "spdlog directory is empty or doesn't exist. Proceeding with installation.")

    # Set up the ExternalProject for spdlog
    include(ExternalProject)

    # Determine the number of available processors in a cross-platform way
    include(ProcessorCount)
    ProcessorCount(NPROC)
    if(NOT NPROC)
        set(NPROC 1)  # Default to 1 if ProcessorCount fails
    endif()

    ExternalProject_Add(spdlog
            PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/spdlog_build
            GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
            GIT_TAG "v1.x"
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spdlog_build/src/spdlog
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${SPDLOG_DIR}
            -DCMAKE_BUILD_TYPE=Release
            BUILD_COMMAND cmake --build . --config Release -- -j${NPROC}
            INSTALL_COMMAND cmake --build . --target install
    )

    # Custom target to clean the build directory after installation
    add_custom_target(clean_spdlog_build ALL
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/spdlog_build
            COMMENT "Cleaning up spdlog_build directory after installation"
    )

    # Ensure the clean target runs after spdlog is built
    add_dependencies(clean_spdlog_build spdlog)
endif()
