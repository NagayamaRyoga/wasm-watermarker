ExternalProject_Add(
    binaryen
    URL https://github.com/WebAssembly/binaryen/archive/version_72.zip
    PREFIX binaryen
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    CMAKE_ARGS
        -DBUILD_STATIC_LIB=OFF
        -DCMAKE_BUILD_TYPE=Release
)

ExternalProject_Get_Property(binaryen source_dir)
ExternalProject_Get_Property(binaryen binary_dir)

file(MAKE_DIRECTORY ${source_dir}/src)

add_library(libbinaryen SHARED IMPORTED)
add_dependencies(libbinaryen binaryen)

if (WIN32)
    set(binaryen_SHARED_LIB ${binary_dir}/bin/libbinaryen${CMAKE_SHARED_LIBRARY_SUFFIX})
else ()
    set(binaryen_SHARED_LIB ${binary_dir}/lib/libbinaryen${CMAKE_SHARED_LIBRARY_SUFFIX})
endif ()

set_target_properties(libbinaryen
    PROPERTIES
    IMPORTED_LOCATION ${binaryen_SHARED_LIB}
    IMPORTED_IMPLIB ${binary_dir}/lib/libbinaryen.dll.a
    INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/src
)
