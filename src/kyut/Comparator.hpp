#ifndef INCLUDE_kyut_Comparator_hpp
#define INCLUDE_kyut_Comparator_hpp

#include <wasm.h>

namespace wasm {
    [[nodiscard]] bool operator<(const Literal &a, const Literal &b);
    [[nodiscard]] bool operator<(const Expression &a, const Expression &b);
    [[nodiscard]] bool operator<(const ExpressionList &a, const ExpressionList &b);
} // namespace wasm

#include "Comparator.inl.hpp"

#endif // INCLUDE_kyut_Comparator_hpp
