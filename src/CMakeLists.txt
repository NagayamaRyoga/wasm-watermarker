add_executable(snpi
    snpi.cpp
)

target_link_libraries(snpi
    cmdline::cmdline
    binaryen::binaryen
    fmtlib::fmt
)

add_executable(pisn
    pisn.cpp
)

target_link_libraries(pisn
    cmdline::cmdline
    binaryen::binaryen
    fmtlib::fmt
)
