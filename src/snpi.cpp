#include <fmt/printf.h>

#include <wasm-io.h>

#include "kyut/CircularBitStreamReader.hpp"
#include "kyut/watermarker/FunctionOrderingWatermarker.hpp"
#include "kyut/watermarker/OperandSwappingWatermarker.hpp"

int main(int argc, char *argv[]) {
    // Parse command line options
    if (argc != 4) {
        fmt::print(std::cerr, "{} <input file> <method> <watermark>\n", argv[0]);

        return 1;
    }

    const std::string inputFile = argv[1];
    const std::string method = argv[2];
    const std::string watermark = argv[3];

    try {
        // Read the input module
        wasm::Module module;
        wasm::ModuleReader{}.read(inputFile, module);

        // Embed watermarks
        const auto stream = kyut::CircularBitStreamReader::fromString(watermark);

        if (method == "opswap") {
            kyut::watermarker::embedOperandSwapping(module, *stream);
        } else if (method == "funcord") {
            kyut::watermarker::embedFunctionOrdering(module, *stream, 10);
        } else {
            throw std::runtime_error{fmt::format("unknown embedding method: {}", method)};
        }

        // Output the result
        wasm::ModuleWriter{}.writeText(module, "");

        fmt::print(std::cerr, "{} bits embedded\n", stream->tell());
    } catch (const wasm::ParseException &e) {
        e.dump(std::cerr);

        return 1;
    } catch (const std::exception &e) {
        fmt::print(std::cerr, "exception: {}\n", e.what());

        return 1;
    } catch (...) {
        fmt::print(std::cerr, "unknown exception\n");

        return 1;
    }
}
