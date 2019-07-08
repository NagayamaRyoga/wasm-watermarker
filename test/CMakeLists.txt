add_executable(test_kyut
    test_kyut.cpp
    test_BitStreamReader.cpp
)

target_link_libraries(test_kyut
    kyut
)
