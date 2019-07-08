add_library(kyut
    kyut/watermarker/FunctionOrderingWatermarker.cpp
)

target_link_libraries(kyut
    fmt::fmt
    binaryen::binaryen
)

add_executable(snpi
    snpi.cpp
)

target_link_libraries(snpi
    kyut
)
