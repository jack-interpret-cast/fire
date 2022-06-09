cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(fire LANGUAGES CXX)

include(cmake/project_settings.cmake)
common_setup()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(FIRE_MASTER_PROJECT TRUE)
endif ()


# Setup Externals
FetchContent_Declare(boost_pfr
        GIT_REPOSITORY https://github.com/boostorg/pfr.git
        GIT_TAG 2.0.3
        GIT_SHALLOW ON
        )
FetchContent_MakeAvailable(boost_pfr)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        GIT_SHALLOW ON
        )
FetchContent_MakeAvailable(fmt)

# Common lib/exe properties
add_library(project_options INTERFACE)
add_library(fire::project_options ALIAS project_options)
target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
target_compile_options(project_options
        INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        )

set(FIRE_SOURCES
        src/parser.cpp
        )
set(FIRE_HEADERS
        include/fire/parser.hpp
        include/fire/types.hpp
        )
add_library(fire
        STATIC
        ${FIRE_SOURCES}
        ${FIRE_HEADERS}
        )
add_library(fire::fire ALIAS fire)

target_link_libraries(fire
        PUBLIC
        Boost::pfr
        fmt::fmt

        PRIVATE
        project_options
        )
target_include_directories(fire PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
        )

# Assume people pulling this in as a dependency don't want to build the tests
if (FIRE_MASTER_PROJECT)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.11.0
            GIT_SHALLOW ON
            )
    FetchContent_MakeAvailable(googletest)


    enable_testing()
    add_subdirectory(test)
endif ()

