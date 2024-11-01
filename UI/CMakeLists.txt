# FPH Deserialization API

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# Set compiler to GCC
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set path for Crow
set(CMAKE_PREFIX_PATH "${DEPENDENCY_DIR}/crow")
set(crow_DIR "${DEPENDENCY_DIR}/crow/lib/cmake/Crow")
cmake_policy(SET CMP0057 NEW)
find_package(Crow REQUIRED)

# Set path for nlohmann_json
set(CMAKE_PREFIX_PATH "${DEPENDENCY_DIR}/nlohmann_json")
set(json_DIR "${DEPENDENCY_DIR}/nlohmann_json/share/cmake/nlohmann_json")
find_package(nlohmann_json REQUIRED)

# Add the subdirectory for Floating-Point-Histogram
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../build)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
    deserialize.cpp
    src/1d.cpp
    src/2d.cpp
)

# Create an executable or library for hmdh_ui
add_executable(hmdh_ui ${SOURCES})

# Link the Floating-Point-Histogram library to hmdh_ui
# target_link_libraries(hmdh_ui PRIVATE serialization oatpp::oatpp)
target_link_libraries(hmdh_ui PRIVATE serialization Crow::Crow nlohmann_json::nlohmann_json)

# If hmdh_ui needs to include headers from Floating-Point-Histogram
target_include_directories(hmdh_ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Install the hmdh_ui executable
install(TARGETS hmdh_ui DESTINATION bin)

# Install the static assets for hmdh_ui
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/static DESTINATION share/hmdh_ui)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION share/hmdh_ui)
