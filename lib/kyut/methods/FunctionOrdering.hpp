#ifndef INCLUDE_kyut_methods_FunctionOrdering_hpp
#define INCLUDE_kyut_methods_FunctionOrdering_hpp

#include "../Ordering.hpp"
#include "wasm.h"

namespace kyut {
    class CircularBitStreamReader;
    class BitStreamWriter;
} // namespace kyut

namespace kyut::methods::function_ordering {
    inline std::size_t embed(CircularBitStreamReader& r, wasm::Module& module, std::size_t chunk_size) {
        const auto begin = std::begin(module.functions);
        const auto end = std::end(module.functions);

        const auto start = std::partition(begin, end, [](const auto& f) {
            return f->body == nullptr;
        });

        const auto size_bits = embed_by_ordering(
            r,
            chunk_size,
            start,
            end,
            [](const auto& a, const auto& b) {
                return a->name < b->name;
            });

        return size_bits;
    }

    inline std::size_t extract(BitStreamWriter& w, wasm::Module& module, std::size_t chunk_size) {
        const auto begin = std::begin(module.functions);
        const auto end = std::end(module.functions);

        const auto start = std::partition(begin, end, [](const auto& f) {
            return f->body == nullptr;
        });

        const auto size_bits = extract_by_ordering(
            w,
            chunk_size,
            start,
            end,
            [](const auto& a, const auto& b) {
                return a->name < b->name;
            });

        return size_bits;
    }
} // namespace kyut::methods::function_ordering

#endif // INCLUDE_kyut_methods_FunctionOrdering_hpp
