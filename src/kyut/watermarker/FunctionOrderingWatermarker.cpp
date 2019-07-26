#include "FunctionOrderingWatermarker.hpp"

#include <algorithm>
#include <array>
#include <cassert>

#include "../BitStreamWriter.hpp"
#include "../CircularBitStreamReader.hpp"

namespace kyut::watermarker {
    namespace {
        // Number of bits that can be embedded in a chunk
        // {floor(log2(x!)) | x in [2, 21)}
        constexpr auto factorialBitLengthTable = std::array<std::size_t, 21>{
            0, 0, 1, 2, 4, 6, 9, 12, 15, 18, 21, 25, 28, 32, 36, 40, 44, 48, 52, 56, 61,
        };

        template <typename Iterator>
        std::size_t embedIntoChunk(Iterator chunkBegin, Iterator chunkEnd, CircularBitStreamReader &stream) {
            const auto chunkSize = std::distance(chunkBegin, chunkEnd);

            // Number of bits that can be embedded in the chunk
            const auto numBits = factorialBitLengthTable[chunkSize];

            // Sort functions in the chunk
            std::sort(chunkBegin, chunkEnd, [](const auto &a, const auto &b) { return a->name < b->name; });

            // A watermark to embed in the chunk
            auto watermark = stream.read<std::uint64_t>(numBits);

            // Reorder the functions
            for (auto it = chunkBegin; it != chunkEnd; ++it) {
                const auto distance = std::distance(it, chunkEnd);
                const auto n = watermark % distance;
                watermark /= distance;

                std::swap(*it, *(it + n));
            }

            return numBits;
        }

        template <typename Iterator>
        std::size_t extractFromChunk(Iterator chunkBegin, Iterator chunkEnd, BitStreamWriter &stream) {
            const auto chunkSize = std::distance(chunkBegin, chunkEnd);

            // Number of bits embedded in the chunk
            const auto numBits = factorialBitLengthTable[chunkSize];

            // Extract watermarks from the chunk
            std::vector<wasm::Function *> functions;
            functions.reserve(chunkSize);

            std::transform(chunkBegin, chunkEnd, std::back_inserter(functions), [](const auto &f) { return f.get(); });

            std::sort(std::begin(functions), std::end(functions), [](const auto &a, const auto &b) {
                return a->name < b->name;
            });

            std::uint64_t watermark = 0;
            std::uint64_t base = 1;

            auto funcBegin = std::begin(functions);
            const auto funcEnd = std::end(functions);

            for (auto it = chunkBegin; it != chunkEnd; ++it, ++funcBegin) {
                // Get index of the function `*it`
                const auto pos = std::find_if(funcBegin, funcEnd, [it](const auto &f) { return f == it->get(); });
                assert(pos != funcEnd);

                const std::size_t index = std::distance(funcBegin, pos);

                watermark += index * base;
                base *= std::distance(funcBegin, funcEnd);

                // Remove the function found in this step
                std::swap(*funcBegin, *pos);
            }

            stream.write(watermark, numBits);

            return numBits;
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

            // Embed watermarks into the chunk
            numBits += embedIntoChunk(chunkBegin, chunkEnd, stream);
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

            // Extract watermarks from the chunk
            numBits += extractFromChunk(chunkBegin, chunkEnd, stream);
        }

        return numBits;
    }
} // namespace kyut::watermarker
