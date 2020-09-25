ExternalProject_Add(binaryen
    URL             "https://github.com/WebAssembly/binaryen/archive/version_97.tar.gz"
    PREFIX          "${CMAKE_CURRENT_BINARY_DIR}/binaryen"
    SOURCE_DIR      "${CMAKE_CURRENT_BINARY_DIR}/binaryen/src"
    BINARY_DIR      "${CMAKE_CURRENT_BINARY_DIR}/binaryen/build"
    STAMP_DIR       "${CMAKE_CURRENT_BINARY_DIR}/binaryen/stamp"
    # UPDATE_COMMAND  ""
    INSTALL_COMMAND ""
    TEST_COMMAND    ""
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DBUILD_STATIC_LIB=ON
        -DCMAKE_BUILD_TYPE=Release
)

ExternalProject_Get_Property(binaryen source_dir)
ExternalProject_Get_Property(binaryen binary_dir)

add_library(binaryen::binaryen STATIC IMPORTED)

make_directory("${source_dir}/src") # To suppress non-exist directory warnings

set_target_properties(binaryen::binaryen PROPERTIES
    IMPORTED_LOCATION "${binary_dir}/lib/libbinaryen.a"
    INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/src"
)

add_dependencies(binaryen::binaryen binaryen)
