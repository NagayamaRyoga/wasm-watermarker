add_library(elder
    kyut.cpp
    kyut/pass/operand_swapping_watermarker.cpp
    kyut/pass/operand_swapping_extractor.cpp
)

target_link_libraries(elder
    binaryen::binaryen
    fmt::fmt
)

add_executable(kyut
    kyut.cpp
)

target_link_libraries(kyut
    elder
)

add_executable(pisn
    pisn.cpp
)

target_link_libraries(pisn
    elder
)
