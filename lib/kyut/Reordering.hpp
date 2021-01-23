#ifndef INCLUDE_kyut_Reordering_hpp
#define INCLUDE_kyut_Reordering_hpp

#include <cstddef>

namespace kyut {
    class CircularBitStreamReader;
    class BitStreamWriter;

    constexpr std::size_t max_chunk_size = 20;

    template <typename RandomAccessIterator, typename Less>
    std::size_t embed_by_reordering(
        CircularBitStreamReader& r,
        std::size_t limit,
        std::size_t chunk_size,
        RandomAccessIterator begin,
        RandomAccessIterator end,
        Less less);

    template <typename RandomAccessIterator, typename Less>
    std::size_t extract_by_reordering(
        BitStreamWriter& w,
        std::size_t chunk_size,
        RandomAccessIterator begin,
        RandomAccessIterator end,
        Less less);
} // namespace kyut

#include "Reordering-inl.hpp"

#endif // INCLUDE_kyut_Reordering_hpp
