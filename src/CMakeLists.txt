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
