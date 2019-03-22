#ifndef INCLUDE_kyut_pass_operand_swapping_watermarker_hpp
#define INCLUDE_kyut_pass_operand_swapping_watermarker_hpp

#include <wasm.h>

#include "../circular_bit_stream.hpp"

namespace kyut::pass {
    void embedWatermarkOperandSwapping(wasm::Module &module, CircularBitStream &stream);
}

#endif // INCLUDE_kyut_pass_operand_swapping_watermarker_hpp
