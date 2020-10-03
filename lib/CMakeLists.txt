add_library(kyut INTERFACE)

target_include_directories(kyut INTERFACE
    "."
)

target_link_libraries(kyut INTERFACE
    binaryen::binaryen
    fmtlib::fmt
)
