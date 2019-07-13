#ifndef INCLUDE_kyut_watermark_OperandSwappingWatermarker_hpp
#define INCLUDE_kyut_watermark_OperandSwappingWatermarker_hpp

#include <wasm.h>

namespace kyut {
    class BitStreamWriter;
    class CircularBitStreamReader;
} // namespace kyut

namespace kyut::watermarker {
    /**
     * @brief   Embed watermarks by operand swapping.
     *
     * @param   module          A reference to the WebAssembly module in which watermarks will be embedded.
     * @param   stream          Input stream containing watermarks to embed.
     * @return  Number of watermark bits embedded in the module.
     */
    std::size_t embedOperandSwapping(wasm::Module &module, CircularBitStreamReader &stream);

    /**
     * @brief Extract watermarks by operand swapping.
     *
     * @param   module          A reference to the WebAssembly module with embedded watermarks.
     * @param   stream          Output stream to save the watermarks.
     * @return  Number of watermark bits extracted from the module.
     */
    std::size_t extractOperandSwapping(wasm::Module &module, BitStreamWriter &stream);
} // namespace kyut::watermarker

#endif // INCLUDE_kyut_watermark_OperandSwappingWatermarker_hpp
