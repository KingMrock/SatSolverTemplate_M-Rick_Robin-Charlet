project(Tests)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/62df7bdbc10887e094661e07ec2595b7920376fd.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include_directories(${TEST_NAME} "${CMAKE_SOURCE_DIR}/Solver")
add_compile_definitions(__TEST_DATA_DIR__="${CMAKE_SOURCE_DIR}/Tests/problems/")
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/Tests/test_*.cpp)
message("generating following tests")
foreach (TEST ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST} NAME_WLE)
    message(\t${TEST}\ ->\ target:\ ${TEST_NAME})
    add_executable(${TEST_NAME} ${TEST} ${SOURCES} "$<$<CONFIG:Debug>:${BACKWARD_ENABLE}>")
    target_link_libraries(${TEST_NAME} gtest gmock "$<$<CONFIG:Debug>:Backward::Interface>")
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach ()

add_executable(all_tests all_tests.cpp ${TEST_SOURCES} ${SOURCES} "$<$<CONFIG:Debug>:${BACKWARD_ENABLE}>")
target_compile_definitions(all_tests PUBLIC __RUN_ALL_TESTS__)
target_link_libraries(all_tests gtest gmock "$<$<CONFIG:Debug>:Backward::Interface>")

add_test(NAME all_tests COMMAND all_tests)
