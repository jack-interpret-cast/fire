macro(common_setup)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # For external tooling
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Faster compilation
    find_program(CCACHE ccache)
    if (${CCACHE} STREQUAL "CCACHE-NOTFOUND")
        message(WARNING "ccache not found")
    else ()
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    endif ()

    # Faster linking
    find_program(ALTERNATIVE_LINKER ld.mold)
    if (${ALTERNATIVE_LINKER} STREQUAL "ALTERNATIVE_LINKER-NOTFOUND")
        message(WARNING "mold linker not found")
    else ()
        if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
            add_link_options("-B/usr/lib/mold")
        else ()
            add_link_options("-fuse-ld=${ALTERNATIVE_LINKER}")
        endif ()
    endif ()

    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED True)
endmacro()

function(setup_utils)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${fire_SOURCE_DIR}/.clang-format ${PROJECT_SOURCE_DIR}/.clang-format)
endfunction()

