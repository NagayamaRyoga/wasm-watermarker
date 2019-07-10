#ifndef INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp
#define INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp

#include <wasm.h>

namespace kyut {
    class BitStreamReader;
}

namespace kyut::watermarker {
    std::size_t embedFunctionOrdering(wasm::Module &module, BitStreamReader &stream, std::size_t divisions);
}

#endif // INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp
