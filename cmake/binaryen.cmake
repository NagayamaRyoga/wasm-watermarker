ExternalProject_Add(
    binaryen
    URL https://github.com/WebAssembly/binaryen/archive/1.38.32.tar.gz
    PREFIX binaryen
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    CMAKE_ARGS
        -DBUILD_STATIC_LIB=ON
        -DCMAKE_BUILD_TYPE=Release
)

ExternalProject_Get_Property(binaryen source_dir)
ExternalProject_Get_Property(binaryen binary_dir)

file(MAKE_DIRECTORY ${source_dir}/src)

add_library(binaryen::binaryen STATIC IMPORTED)
add_dependencies(binaryen::binaryen binaryen)

set(binaryen_LIBS
    ${binary_dir}/lib/libwasm.a
    ${binary_dir}/lib/libasmjs.a
    ${binary_dir}/lib/libpasses.a
    ${binary_dir}/lib/libcfg.a
    ${binary_dir}/lib/libir.a
    ${binary_dir}/lib/libemscripten-optimizer.a
    ${binary_dir}/lib/libsupport.a
    ${binary_dir}/lib/libwasm.a
    Threads::Threads
)

set_target_properties(binaryen::binaryen
    PROPERTIES
    IMPORTED_LOCATION ${binary_dir}/lib/libpasses.a
    INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/src
    INTERFACE_LINK_LIBRARIES "${binaryen_LIBS}"
)
