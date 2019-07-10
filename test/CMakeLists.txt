add_executable(test_kyut
    test_kyut.cpp
    kyut/test_BitStreamReader.cpp
    kyut/watermarker/test_FunctionOrderingWatermarker.cpp
)

target_link_libraries(test_kyut
    kyut
)
