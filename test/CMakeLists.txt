add_executable(test_kyut
    test_kyut.cpp
    kyut/test_CircularBitStreamReader.cpp
    kyut/test_BitStreamWriter.cpp
    kyut/watermarker/test_FunctionOrderingWatermarker.cpp
    kyut/watermarker/test_OperandSwappingWatermarker.cpp
)

target_compile_definitions(test_kyut
    PRIVATE KYUT_TEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(test_kyut
    kyut
)
