#ifndef INCLUDE_kyut_wasm_ext_Compare_hpp
#define INCLUDE_kyut_wasm_ext_Compare_hpp

#include "wasm.h"

namespace wasm {
    bool operator<(const Literal& a, const Literal& b);
    bool operator<(const Expression& a, const Expression& b);
    bool operator<(const ExpressionList& a, const ExpressionList& b);
    bool operator<(const Function& a, const Function& b);
} // namespace wasm

#include "Compare-inl.hpp"

#endif // INCLUDE_kyut_wasm_ext_Compare_hpp
