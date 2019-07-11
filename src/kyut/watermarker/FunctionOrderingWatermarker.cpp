#include "FunctionOrderingWatermarker.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

#include "../BitStreamWriter.hpp"
#include "../CircularBitStreamReader.hpp"

namespace kyut::watermarker {
    namespace {
        // Number of bits that can be embedded in a chunk
        // {floor(log2(x!)) | x in [2, 21)}
        constexpr auto factorialBitLengthTable = std::array<std::size_t, 21>{
            0, 0, 1, 2, 4, 6, 9, 12, 15, 18, 21, 25, 28, 32, 36, 40, 44, 48, 52, 56, 61,
        };

        /**
         * @brief   Move the `element` in [begin, end) to the beginning of the range.
         *          The order of elements other than `element` is maintained.
         *
         * @tparam  BidirectionalIterator   Bidirectional iterator type.
         * @param   begin       Bidirectional iterator to the initial position in a sequence.
         * @param   end         Bidirectional iterator to the final position in a sequence.
         * @param   element     Bidirectional iterator to the element to be moved.
         *                      `element` must be in the range of [begin, end).
         */
        template <typename BidirectionalIterator>
        void moveToFront(BidirectionalIterator begin, BidirectionalIterator end, BidirectionalIterator element) {
            assert(std::distance(begin, element) >= 0);
            assert(std::distance(begin, element) < std::distance(begin, end));

            auto tmp = std::move(*element);
            std::move_backward(begin, element, std::next(element));
            *begin = std::move(tmp);
        }
    } // namespace

    std::size_t embedFunctionOrdering(wasm::Module &module, CircularBitStreamReader &stream, std::size_t maxChunkSize) {
        assert(2 <= maxChunkSize && maxChunkSize < 21 && "because 21! > 2^64");

        // Number of bits embedded in the module
        std::size_t numBits = 0;

        // Split according to the function in the module has body or not
        // [begin, start) has no body, and [start, end) has
        const auto start = std::partition(
            std::begin(module.functions), std::end(module.functions), [](const auto &f) { return f->body == nullptr; });

        const size_t count = std::distance(start, std::end(module.functions));

        for (size_t i = 0; i < count; i += maxChunkSize) {
            const auto chunkSize = (std::min)(maxChunkSize, count - i);
            const auto chunkBegin = start + i;
            const auto chunkEnd = chunkBegin + chunkSize;

            // Number of bits that can be embedded in the chunk
            const auto numBitsEmbeddedInChunk = factorialBitLengthTable[chunkSize];

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

    std::size_t extractFunctionOrdering(wasm::Module &module, BitStreamWriter &stream, std::size_t maxChunkSize) {
        assert(2 <= maxChunkSize && maxChunkSize < 21 && "because 21! > 2^64");

        // Number of bits extracted in the module
        std::size_t numBits = 0;

        // Split according to the function in the module has body or not
        // [begin, start) has no body, and [start, end) has
        const auto start = std::partition(
            std::begin(module.functions), std::end(module.functions), [](const auto &f) { return f->body == nullptr; });

        const size_t count = std::distance(start, std::end(module.functions));

        for (size_t i = 0; i < count; i += maxChunkSize) {
            const auto chunkSize = (std::min)(maxChunkSize, count - i);
            const auto chunkBegin = start + i;
            const auto chunkEnd = chunkBegin + chunkSize;

            // Number of bits embedded in the chunk
            const auto numBitsEmbeddedInChunk = factorialBitLengthTable[chunkSize];

            // Extract watermarks from the chunk
            std::vector<wasm::Function *> functions;
            functions.reserve(chunkSize);

            std::transform(chunkBegin, chunkEnd, std::back_inserter(functions), [](const auto &f) { return f.get(); });

            std::sort(std::begin(functions), std::end(functions), [](const auto &a, const auto &b) {
                return a->name < b->name;
            });

            std::int64_t watermark = 0;
            std::int64_t base = 1;

            for (auto it = chunkBegin; it != chunkEnd; ++it) {
                // Get index of the function `*it`
                const auto pos = std::find_if(
                    std::begin(functions), std::end(functions), [it](const auto &f) { return f == it->get(); });
                assert(pos != std::end(functions));

                const std::size_t index = std::distance(std::begin(functions), pos);

                watermark += index * base;
                base *= functions.size();

                // Remove the function found in this step
                functions.erase(pos);
            }

            stream.write(watermark, numBitsEmbeddedInChunk);

            numBits += numBitsEmbeddedInChunk;
        }

        return numBits;
    }
} // namespace kyut::watermarker
