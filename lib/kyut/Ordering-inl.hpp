#ifndef INCLUDE_kyut_Ordering_inl_hpp
#define INCLUDE_kyut_Ordering_inl_hpp

#include "Ordering.hpp"

#include <algorithm>
#include "CircularBitStreamReader.hpp"

namespace kyut {
    namespace detail {
        // {log2(n!) | x in [2, 20]}
        constexpr std::size_t factorial_bit_width_table[max_chunk_size + 1] = {
            0, 0, 1, 2, 4, 6, 9, 12, 15, 18, 21, 25, 28, 32, 36, 40, 44, 48, 52, 56, 61};

        template <typename RandomAccessIterator, typename Less>
        inline void embed_in_chunk(
            std::uint64_t watermark,
            RandomAccessIterator begin,
            RandomAccessIterator end,
            Less less) {
            assert(std::distance(begin, end) >= 0);

            std::sort(begin, end, less);

            for (auto it = begin; it != end; ++it) {
                const std::size_t count = std::distance(it, end);

                const std::uint64_t w = watermark % count;
                watermark /= count;

                std::iter_swap(it, it + w);
            }
        }

        template <typename RandomAccessIterator, typename Less>
        inline std::size_t embed_by_ordering(
            CircularBitStreamReader& r,
            std::size_t chunk_size,
            RandomAccessIterator begin,
            RandomAccessIterator end,
            Less less) {
            assert(2 <= chunk_size && chunk_size <= max_chunk_size);
            assert(std::distance(begin, end) >= 0);

            const std::size_t count = std::distance(begin, end);

            std::size_t size_bits = 0;
            for (std::size_t i = 0; i < count; i += chunk_size) {
                const std::size_t chunk_size = (std::min)(chunk_size, count - i);
                const auto chunk_begin = begin + i;
                const auto chunk_end = chunk_begin + chunk_size;

                const auto bit_width = factorial_bit_width_table[chunk_size];
                const auto watermark = r.read(bit_width);

                embed_in_chunk(watermark, chunk_begin, chunk_end, less);

                size_bits += bit_width;
            }

            return size_bits;
        }
    } // namespace detail

    template <typename RandomAccessIterator, typename Less>
    inline std::size_t embed_by_ordering(
        CircularBitStreamReader& r,
        std::size_t chunk_size,
        RandomAccessIterator begin,
        RandomAccessIterator end,
        Less less) {
        return detail::embed_by_ordering(r, chunk_size, begin, end, less);
    }
} // namespace kyut

#endif // INCLUDE_kyut_Ordering_inl_hpp
