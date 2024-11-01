# src/CMakeLists.txt

# Compiler-specific options
if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

# Set the source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# List all source files
set(SOURCE_FILES
        ${SRC_DIR}/ExportUtilities/1-D.cpp

        ${SRC_DIR}/FloatingPointTrie/1-D.cpp
        ${SRC_DIR}/FloatingPointTrie/2-D.cpp

        ${SRC_DIR}/FloatingPointUtilities/1-D.cpp
        ${SRC_DIR}/FloatingPointUtilities/2-D.cpp
        ${SRC_DIR}/FloatingPointUtilities/ND.cpp

        ${SRC_DIR}/TrieOperation/1-D.cpp
        ${SRC_DIR}/TrieOperation/2-D.cpp


        ${SRC_DIR}/TrieSerialization/1-D.cpp
        ${SRC_DIR}/TrieSerialization/2-D.cpp

        ${SRC_DIR}/BooleanArray.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Generate a library
add_library(serialization STATIC
        ${SRC_DIR}/TrieSerialization/1-D.cpp
        ${SRC_DIR}/TrieSerialization/2-D.cpp
        ${SRC_DIR}/FloatingPointUtilities/ND.cpp
        ${SRC_DIR}/FloatingPointTrie/1-D.cpp
        ${SRC_DIR}/FloatingPointTrie/2-D.cpp
        ${SRC_DIR}/BooleanArray.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
# Link HMDH with LibUtilities
target_link_libraries(${PROJECT_NAME} PRIVATE LibUtilities)

## Install the static library to /usr/local/lib
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)
## Install the header files to /usr/local/include
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)


