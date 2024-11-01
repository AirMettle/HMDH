

if (OS STREQUAL "LINUX" AND ARCH STREQUAL "x86_64")
    set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
    set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib")

elseif (OS STREQUAL "LINUX" AND ARCH STREQUAL "aarch64")
    if (DISTRO STREQUAL "ubuntu")
        set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
        set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib")
    else ()
        set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
        set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib64")
    endif ()

elseif (OS STREQUAL "DARWIN" AND ARCH STREQUAL "aarch64")
    set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
    set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib")
else ()
    message(FATAL_ERROR "Unsupported combination of OS and architecture")
endif ()

find_package(GTest REQUIRED)

if(GTEST_FOUND)
    message(STATUS "Google Test found at ${GTEST_INCLUDE_DIR}")
else()
    message(WARNING "Google Test not found")
endif()

add_executable(ut_serialization_test UnitTest/SerializationTest.cpp utils/utils.cpp)
target_link_libraries(ut_serialization_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main OpenSSL::SSL OpenSSL::Crypto)
add_test(NAME ut_serialization_test COMMAND ut_serialization_test)

add_executable(ut_headerdeserialization_test UnitTest/HeaderDeserializationTest.cpp)
target_link_libraries(ut_headerdeserialization_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
add_test(NAME ut_headerdeserialization_test COMMAND ut_headerdeserialization_test)

add_executable(ut_fpisSpecial_test UnitTest/SpecialCaseTest.cpp)
target_link_libraries(ut_fpisSpecial_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
add_test(NAME ut_fpisSpecial_test COMMAND ut_fpisSpecial_test)

#TODO : Change name of these tests
add_executable(ut_fpinternalmanual_test UnitTest/FpInternalManualTest.cpp)
target_link_libraries(ut_fpinternalmanual_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
add_test(NAME ut_fpinternalmanual_test COMMAND ut_fpinternalmanual_test)


add_executable(ut_fpinternal_1D_test UnitTest/FpInternalTest_1D.cpp)
target_link_libraries(ut_fpinternal_1D_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
add_test(NAME ut_fpinternal_1D_test COMMAND ut_fpinternal_1D_test)

# # FUNCTIONAL TESTS HERE
add_executable(fn_1D_test FnTest/1-D.cpp utils/utils.cpp)
target_link_libraries(fn_1D_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main OpenSSL::SSL OpenSSL::Crypto)
add_test(NAME fn_1D_test COMMAND fn_1D_test)


add_executable(fn_2D_test FnTest/2-D.cpp utils/utils.cpp)
target_link_libraries(fn_2D_test PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main OpenSSL::SSL OpenSSL::Crypto)
add_test(NAME fn_2D_test COMMAND fn_2D_test)
