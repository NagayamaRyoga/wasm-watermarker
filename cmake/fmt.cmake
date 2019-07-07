ExternalProject_Add(
    fmt
    URL https://github.com/fmtlib/fmt/archive/5.3.0.tar.gz
    PREFIX fmt
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DFMT_DOC=OFF
        -DFMT_TEST=OFF
)

ExternalProject_Get_Property(fmt source_dir)
ExternalProject_Get_Property(fmt binary_dir)

file(MAKE_DIRECTORY ${source_dir}/include)

add_library(fmt::fmt STATIC IMPORTED)
add_dependencies(fmt::fmt fmt)

set_target_properties(fmt::fmt
    PROPERTIES
    IMPORTED_LOCATION ${binary_dir}/libfmt.a
    INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/include
)
