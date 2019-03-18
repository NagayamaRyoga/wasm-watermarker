#ifndef INCLUDE_kyut_pass_operand_swapping_watermarker_hpp
#define INCLUDE_kyut_pass_operand_swapping_watermarker_hpp

#include <wasm.h>

namespace kyut::pass {
    void embedWatermarkOperandSwapping(wasm::Module &module);
}

#endif // INCLUDE_kyut_pass_operand_swapping_watermarker_hpp
