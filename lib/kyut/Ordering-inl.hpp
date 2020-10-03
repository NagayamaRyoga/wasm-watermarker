#ifndef INCLUDE_kyut_Ordering_inl_hpp
#define INCLUDE_kyut_Ordering_inl_hpp

#include "Ordering.hpp"

#include <algorithm>
#include "BitStreamWriter.hpp"
#include "CircularBitStreamReader.hpp"

namespace kyut {
    namespace detail {
        // {log2(n!) | x in [2, 20]}
        constexpr std::size_t factorial_bit_width_table[max_chunk_size + 1] = {
            0, 0, 1, 2, 4, 6, 9, 12, 15, 18, 21, 25, 28, 32, 36, 40, 44, 48, 52, 56, 61};

        template <typename RandomAccessIterator, typename Less>
        inline std::size_t embed_in_chunk(
            CircularBitStreamReader& r,
            RandomAccessIterator begin,
            RandomAccessIterator end,
            Less less) {
            assert(std::distance(begin, end) >= 0);
            assert(std::distance(begin, end) <= std::ptrdiff_t{max_chunk_size});

            const std::size_t count = std::distance(begin, end);
            const auto bit_width = factorial_bit_width_table[count];
            std::uint64_t watermark = r.read(bit_width);

            // Sort the chunk.
            std::sort(begin, end, less);

            // Embed watermark.
            for (std::size_t i = 0; i < count; i++) {
                const std::uint64_t w = watermark % (count - i);
                watermark /= (count - i);

                const auto it = begin + i;
                std::iter_swap(it, it + w);
            }

            return bit_width;
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

                size_bits += embed_in_chunk(r, chunk_begin, chunk_end, less);
            }

            return size_bits;
        }

        template <typename RandomAccessIterator, typename Less>
        inline std::size_t extract_from_chunk(
            BitStreamWriter& w,
            RandomAccessIterator begin,
            RandomAccessIterator end,
            Less less) {
            assert(std::distance(begin, end) >= 0);
            assert(std::distance(begin, end) <= std::ptrdiff_t{max_chunk_size});

            const std::size_t count = std::distance(begin, end);

            // Sort the chunk.
            std::vector<RandomAccessIterator> chunk{};
            chunk.reserve(count);

            for (auto it = begin; it != end; it++) {
                chunk.emplace_back(it);
            }

            std::sort(std::begin(chunk), std::end(chunk), [&less](const RandomAccessIterator& a, const RandomAccessIterator& b) {
                return less(*a, *b);
            });

            // Extract watermark.
            std::uint64_t watermark = 0;
            std::uint64_t base = 1;
            for (std::size_t i = 0; i < count; i++) {
                const auto it = std::begin(chunk) + i;

                // Find the position of `it`.
                const auto found = std::find(it, std::end(chunk), begin + i);
                assert(found != std::end(chunk));

                const std::size_t pos = std::distance(it, found);

                watermark += pos * base;
                base *= count - i;

                // Remove `it` found in this step.
                std::iter_swap(it, found);
            }

            const auto bit_width = factorial_bit_width_table[count];
            w.write(watermark, bit_width);

            return bit_width;
        }

        template <typename RandomAccessIterator, typename Less>
        inline std::size_t extract_by_ordering(
            BitStreamWriter& w,
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

                size_bits += extract_from_chunk(w, chunk_begin, chunk_end, less);
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

    template <typename RandomAccessIterator, typename Less>
    inline std::size_t extract_by_ordering(
        BitStreamWriter& w,
        std::size_t chunk_size,
        RandomAccessIterator begin,
        RandomAccessIterator end,
        Less less) {
        return detail::extract_by_ordering(w, chunk_size, begin, end, less);
    }
} // namespace kyut

#endif // INCLUDE_kyut_Ordering_inl_hpp
