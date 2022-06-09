add_executable(fire_unit_tests
        parser_test.cpp
        )
target_include_directories(fire_unit_tests PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        )
target_link_libraries(fire_unit_tests
        PUBLIC
        gtest_main
        fmt::fmt
        fire

        PRIVATE
        project_options
        )

include(GoogleTest)
gtest_discover_tests(fire_unit_tests)