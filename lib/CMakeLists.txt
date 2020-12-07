add_library(kyut STATIC
    kyut/methods/OperandSwapping.cpp
)

target_include_directories(kyut INTERFACE
    "."
)

target_link_libraries(kyut
    binaryen::binaryen
    fmtlib::fmt
)
