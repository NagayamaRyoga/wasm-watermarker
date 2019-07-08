#ifndef INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp
#define INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp

#include <wasm.h>

#include "../BitStreamReader.hpp"

namespace kyut::watermarker {
    void embedFunctionOrdering(wasm::Module &module, BitStreamReader &stream);
}

#endif // INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp
