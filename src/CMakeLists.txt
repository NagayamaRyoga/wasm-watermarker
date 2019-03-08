add_executable(kyut
    kyut.cpp
)

target_link_libraries(kyut
    binaryen_wasm
)
