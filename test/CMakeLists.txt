add_executable(test_kyut
    test_kyut.cpp
    kyut/test_CircularBitStreamReader.cpp
    kyut/test_BitStreamWriter.cpp
    kyut/watermarker/test_FunctionOrderingWatermarker.cpp
)

target_link_libraries(test_kyut
    kyut
)
