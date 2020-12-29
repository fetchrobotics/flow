#############################################################################
# Create one executable for each unit test file
#############################################################################

file(GLOB UNIT_TEST_FILES "flow/**.cpp" "flow/**/**.cpp")

foreach(file ${UNIT_TEST_FILES})
    get_filename_component(testcase ${file} NAME_WE)

    add_executable(${testcase} "gtest-main.cpp" ${file})

    target_link_libraries(${testcase} flow gtest_main)

    add_test(NAME "${testcase}"
        COMMAND ${testcase} --no-skip
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endforeach()
