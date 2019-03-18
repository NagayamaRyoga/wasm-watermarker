add_executable(kyut
    kyut.cpp
    kyut/pass/operand_swapping_watermarker.cpp
)

target_link_libraries(kyut
    binaryen::binaryen
    fmt::fmt
)
