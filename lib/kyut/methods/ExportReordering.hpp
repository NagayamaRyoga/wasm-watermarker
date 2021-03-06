#ifndef INCLUDE_kyut_methods_ExportReordering_hpp
#define INCLUDE_kyut_methods_ExportReordering_hpp

#include "../Reordering.hpp"
#include "wasm.h"

namespace kyut {
    class CircularBitStreamReader;
    class BitStreamWriter;
} // namespace kyut

namespace kyut::methods::export_reordering {
    inline std::size_t embed(CircularBitStreamReader& r, wasm::Module& module, std::size_t limit, std::size_t chunk_size) {
        const auto size_bits = embed_by_reordering(
            r,
            limit,
            chunk_size,
            std::begin(module.exports),
            std::end(module.exports),
            [](const auto& a, const auto& b) {
                return a->name < b->name;
            });

        return size_bits;
    }

    inline std::size_t extract(BitStreamWriter& w, wasm::Module& module, std::size_t chunk_size) {
        const auto size_bits = extract_by_reordering(
            w,
            chunk_size,
            std::begin(module.exports),
            std::end(module.exports),
            [](const auto& a, const auto& b) {
                return a->name < b->name;
            });

        return size_bits;
    }
} // namespace kyut::methods::export_reordering

#endif // INCLUDE_kyut_methods_ExportReordering_hpp
