ExternalProject_Add(
	binaryen
	URL https://github.com/WebAssembly/binaryen/archive/version_72.zip
	PREFIX binaryen
	INSTALL_COMMAND ""
	TEST_COMMAND ""
	LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(binaryen source_dir)
ExternalProject_Get_Property(binaryen binary_dir)

add_library(binaryen_wasm STATIC IMPORTED)
add_dependencies(binaryen_wasm binaryen)

set_target_properties(binaryen_wasm
	PROPERTIES
	IMPORTED_LOCATION ${binary_dir}/lib/libwasm.a
	INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/src
)
