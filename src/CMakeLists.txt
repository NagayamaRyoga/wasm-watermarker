# add_library(kthr
# )

# target_link_libraries(kthr
#     binaryen::binaryen
#     fmt::fmt
# )

add_executable(kyut
    kyut.cpp
)

target_link_libraries(kyut
    fmt::fmt
)
