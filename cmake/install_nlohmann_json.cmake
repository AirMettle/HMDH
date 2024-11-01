# Define base directory for dependencies
set(DEPENDENCY_DIR "$ENV{HOME}/.hmdh-deps")

# Define the installation directory for nlohmann/json
set(NLOHMANN_JSON_DIR ${DEPENDENCY_DIR}/nlohmann_json)

# Ensure the dependencies directory exists
if(NOT EXISTS ${NLOHMANN_JSON_DIR})
    message(STATUS "Creating directory ${NLOHMANN_JSON_DIR} for nlohmann/json installation...")
    file(MAKE_DIRECTORY ${NLOHMANN_JSON_DIR})
else()
    message(STATUS "Directory ${NLOHMANN_JSON_DIR} already exists.")
endif()

# Check if the nlohmann/json directory is non-empty
file(GLOB NLOHMANN_JSON_CONTENTS "${NLOHMANN_JSON_DIR}/*")
if(NLOHMANN_JSON_CONTENTS)
    message(STATUS "nlohmann/json is already installed. Skipping installation")
else()
    message(STATUS "nlohmann/json directory is empty or doesn't exist. Proceeding with installation")

    # Set up the ExternalProject for nlohmann/json
    include(ExternalProject)

    # Determine the number of available processors in a cross-platform way
    include(ProcessorCount)
    ProcessorCount(NPROC)
    if(NOT NPROC)
        set(NPROC 1)  # Default to 1 if ProcessorCount fails
    endif()

    # Add the ExternalProject to download and install Google Test
    ExternalProject_Add(nlohmann_json
            PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json_build
            GIT_REPOSITORY "https://github.com/nlohmann/json.git"
            GIT_TAG "v3.11.3"
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${NLOHMANN_JSON_DIR}
            -DCMAKE_BUILD_TYPE=Release
            BUILD_COMMAND make -j${NPROC}
    )

    # Custom target to clean the build directory after installation
    add_custom_target(clean_nlohmann_json_build ALL
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json_build
            COMMENT "Cleaning up nlohmann_json_build directory after installation"
    )

    # Ensure the clean target runs after nlohmann/json is built
    add_dependencies(clean_nlohmann_json_build nlohmann_json)
endif()
