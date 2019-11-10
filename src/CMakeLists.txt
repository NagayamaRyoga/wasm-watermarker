add_library(kyut
    kyut/watermarker/FunctionOrderingWatermarker.cpp
    kyut/watermarker/OperandSwappingWatermarker.cpp
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

add_executable(pisn
    pisn.cpp
)

target_link_libraries(pisn
    kyut
)

add_executable(funccnt
    funccnt.cpp
)

target_link_libraries(funccnt
    kyut
)
