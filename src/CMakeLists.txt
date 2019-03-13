add_executable(kyut
    kyut.cpp
)

target_link_libraries(kyut
    libbinaryen
    libfmt
)

if (WIN32)
    add_custom_command(TARGET kyut POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${binaryen_SHARED_LIB}
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif ()
