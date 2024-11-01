# lib/CMakeLists.txt

# Set the source directory
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# List all source files in the lib directory
set(LIB_HEADER_FILES
        ${LIB_DIR}/include/logger.h)
set(LIB_SOURCE_FILES "")

add_library(LibUtilities STATIC ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})
target_include_directories(LibUtilities PUBLIC ${LIB_DIR}/include)
# Force the language if LIB_SOURCE_FILES is empty
set_target_properties(LibUtilities PROPERTIES LINKER_LANGUAGE CXX)
