set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")

add_executable(test_kyut
    test_kyut.cpp
)

target_link_libraries(test_kyut
    Threads::Threads
    fmtlib::fmt
    googletest::gtest
    googletest::gtest_main
)

add_test(NAME tests::kyut
    COMMAND $<TARGET_FILE:test_kyut>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
