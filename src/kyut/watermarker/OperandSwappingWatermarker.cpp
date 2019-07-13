#include "OperandSwappingWatermarker.hpp"

namespace kyut::watermarker {

    std::size_t embedOperandSwapping(wasm::Module &module, CircularBitStreamReader &stream) {
        (void)module;
        (void)stream;
        return 0;
    }

    std::size_t extractOperandSwapping(wasm::Module &module, BitStreamWriter &stream) {
        (void)module;
        (void)stream;
        return 0;
    }
} // namespace kyut::watermarker
