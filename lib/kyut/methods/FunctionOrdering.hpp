#ifndef INCLUDE_kyut_methods_FunctionOrdering_hpp
#define INCLUDE_kyut_methods_FunctionOrdering_hpp

#include "../Ordering.hpp"
#include "wasm.h"

namespace kyut {
    class CircularBitStreamReader;
    class BitStreamWriter;
} // namespace kyut

namespace kyut::methods {
    std::size_t embed_by_function_ordering(CircularBitStreamReader& r, wasm::Module& module, std::size_t chunk_size) {
        const auto begin = std::begin(module.functions);
        const auto end = std::end(module.functions);

        const auto start = std::partition(begin, end, [](const auto& f) {
            return f->body == nullptr;
        });

        return embed_by_ordering(
            r,
            chunk_size,
            start,
            end,
            [](const auto& a, const auto& b) {
                return a->name < b->name;
            });
    }
} // namespace kyut::methods

#endif // INCLUDE_kyut_methods_FunctionOrdering_hpp
