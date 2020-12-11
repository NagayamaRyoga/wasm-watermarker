add_executable(snpi
    snpi.cpp
)

target_link_libraries(snpi
    kyut
    cmdline::cmdline
)

add_executable(pisn
    pisn.cpp
)

target_link_libraries(pisn
    kyut
    cmdline::cmdline
)

add_executable(wasm-stat
    wasm-stat.cpp
)

target_link_libraries(wasm-stat
    binaryen::binaryen
    cmdline::cmdline
    fmtlib::fmt
)
