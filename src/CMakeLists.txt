# add_library(kthr
# )

# target_link_libraries(kthr
#     binaryen::binaryen
#     fmt::fmt
# )

add_executable(snpi
    snpi.cpp
)

target_link_libraries(snpi
    fmt::fmt
    binaryen::binaryen
)
