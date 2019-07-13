#ifndef INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp
#define INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp

#include <wasm.h>

namespace kyut {
    class BitStreamWriter;
    class CircularBitStreamReader;
} // namespace kyut

namespace kyut::watermarker {
    /**
     * @brief   Embed watermarks by changing the order of functions.
     *
     * @param   module          A reference to the WebAssembly module in which watermarks will be embedded.
     * @param   stream          Input stream containing watermarks to embed.
     * @param   maxChunkSize    The maximum number of functions in the watermark chunk.
     * @return  Number of watermark bits embedded in the module.
     */
    std::size_t embedFunctionOrdering(wasm::Module &module, CircularBitStreamReader &stream, std::size_t maxChunkSize);

    /**
     * @brief Extract watermarks by changing the order of functions.
     *
     * @param   module          A reference to the WebAssembly module with embedded watermarks.
     * @param   stream          Output stream to save the watermarks.
     * @param   maxChunkSize    The maximum number of functions in the watermark chunk.
     * @return  Number of watermark bits extracted from the module.
     */
    std::size_t extractFunctionOrdering(wasm::Module &module, BitStreamWriter &stream, std::size_t maxChunkSize);
} // namespace kyut::watermarker

#endif // INCLUDE_kyut_watermark_FunctionOrderingWatermarker_hpp
