#ifndef INCLUDE_kyut_methods_OperandSwapping_hpp
#define INCLUDE_kyut_methods_OperandSwapping_hpp

#include <cstddef>

namespace wasm {
    class Module;
} // namespace wasm

namespace kyut {
    class CircularBitStreamReader;
    class BitStreamWriter;
} // namespace kyut

namespace kyut::methods::operand_swapping {
    std::size_t embed(CircularBitStreamReader& r, wasm::Module& module, std::size_t limit);

    std::size_t extract(BitStreamWriter& w, wasm::Module& module);
} // namespace kyut::methods::operand_swapping

#endif // INCLUDE_kyut_methods_OperandSwapping_hpp
