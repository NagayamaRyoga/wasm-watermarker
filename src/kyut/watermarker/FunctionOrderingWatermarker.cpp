#include "FunctionOrderingWatermarker.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

#include "../BitStreamReader.hpp"

namespace kyut::watermarker {
    namespace {
        /**
         * Move the `element` in [begin, end) to the beginning of the range.
         * The order of elements other than `element` is maintained.
         */
        template <typename RandomAccessIterator>
        void moveToFront(RandomAccessIterator begin, RandomAccessIterator end, RandomAccessIterator element) {
            assert(begin <= element && element < end);

            auto tmp = std::move(*element);
            std::move_backward(begin, element, std::next(element));
            *begin = std::move(tmp);
        }
    } // namespace

    std::size_t embedFunctionOrdering(wasm::Module &module, BitStreamReader &stream, std::size_t divisions) {
        assert(2 <= divisions && divisions < 21 && "because 21! > 2^64");

        // Number of bits embedded in the module
        std::size_t numBits = 0;

        // Split according to the function in the module has body or not
        // [begin, start) has no body, and [start, end) has
        const auto start = std::partition(
            std::begin(module.functions), std::end(module.functions), [](const auto &f) { return f->body == nullptr; });

        const size_t count = std::distance(start, std::end(module.functions));

        for (size_t i = 0; i < count; i += divisions) {
            const auto chunkSize = (std::min)(divisions, count - i);
            const auto chunkBegin = start + i;
            const auto chunkEnd = chunkBegin + chunkSize;

            // Number of bits that can be embedded in the chunk
            // {floor(log2(x!)) | x in [0, 21)}
            constexpr auto bitLengthTable = std::array<std::size_t, 21>{
                0, 0, 1, 2, 4, 6, 9, 12, 15, 18, 21, 25, 28, 32, 36, 40, 44, 48, 52, 56, 61,
            };
            const auto numBitsEmbeddedInChunk = bitLengthTable[chunkSize];

            // Sort functions in the chunk
            std::sort(chunkBegin, chunkEnd, [](const auto &a, const auto &b) { return a->name < b->name; });

            // A watermark to embed in the chunk
            auto watermark = stream.read(numBitsEmbeddedInChunk);

            // Reorder the functions
            for (auto it = chunkBegin; it != chunkEnd; ++it) {
                const auto distance = std::distance(it, chunkEnd);
                const auto n = watermark % distance;
                watermark /= distance;

                moveToFront(it, chunkEnd, it + n);
            }

            numBits += numBitsEmbeddedInChunk;
        }

        return numBits;
    }
} // namespace kyut::watermarker
