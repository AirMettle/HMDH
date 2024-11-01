# include/CMakeLists.txt

# Add the top-level directory
include_directories(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Add the subdirectories as include directories
include_directories(
        ${INCLUDE_DIR}
        ${INCLUDE_DIR}/ExportUtilities
        ${INCLUDE_DIR}/FloatingPointTrie
        ${INCLUDE_DIR}/FloatingPointUtilities
        ${INCLUDE_DIR}/TrieOperation
        ${INCLUDE_DIR}/TrieSerialization
)

#Listing all the headers as it might be useful for IDE's
set(HEADER_FILES
        ${INCLUDE_DIR}/BooleanArray.h
        ${INCLUDE_DIR}/FeatureFlags.h

        ${INCLUDE_DIR}/ExportUtilities/1-D.h

        ${INCLUDE_DIR}/HMDH.h

        ${INCLUDE_DIR}/FloatingPointTrie/1-D.h
        ${INCLUDE_DIR}/FloatingPointTrie/2-D.h
        ${INCLUDE_DIR}/FloatingPointTrie/ND.h

        ${INCLUDE_DIR}/FloatingPointUtilities/1-D.h
        ${INCLUDE_DIR}/FloatingPointUtilities/2-D.h
        ${INCLUDE_DIR}/FloatingPointUtilities/ND.h

        ${INCLUDE_DIR}/TrieOperation/1-D.h
        ${INCLUDE_DIR}/TrieOperation/2-D.h

        ${INCLUDE_DIR}/TrieSerialization/1-D.h
        ${INCLUDE_DIR}/TrieSerialization/2-D.h
)

#Export this list to the parent CMakeLists.txt in-case we need it
set(PROJECT_HEADER_FILES ${HEADER_FILES} PARENT_SCOPE)