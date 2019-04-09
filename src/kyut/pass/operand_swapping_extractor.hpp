#ifndef INCLUDE_kyut_pass_operand_swapping_extractor_cpp
#define INCLUDE_kyut_pass_operand_swapping_extractor_cpp

#include <wasm.h>

#include "../bit_writer.hpp"

namespace kyut::pass {
    void extractWatermarkOperandSwapping(wasm::Module &module, BitWriter &writer);
}

#endif // INCLUDE_kyut_pass_operand_swapping_extractor_cpp
