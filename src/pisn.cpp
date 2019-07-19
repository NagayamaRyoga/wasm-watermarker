#include <fmt/printf.h>

#include <wasm-io.h>

#include "kyut/BitStreamWriter.hpp"
#include "kyut/watermarker/FunctionOrderingWatermarker.hpp"
#include "kyut/watermarker/OperandSwappingWatermarker.hpp"

int main(int argc, char *argv[]) {
    // Parse command line options
    if (argc != 3) {
        fmt::print(std::cerr, "{} <input file> <method>\n", argv[0]);

        return 1;
    }

    const std::string inputFile = argv[1];
    const std::string method = argv[2];

    try {
        // Read the input module
        wasm::Module module;
        wasm::ModuleReader{}.read(inputFile, module);

        // Embed watermarks
        kyut::BitStreamWriter stream;

        if (method == "opswap") {
            kyut::watermarker::extractOperandSwapping(module, stream);
        } else if (method == "funcord") {
            kyut::watermarker::extractFunctionOrdering(module, stream, 10);
        } else {
            throw std::runtime_error{fmt::format("unknown embedding method: {}", method)};
        }

        // Output watermarks extracted
        fmt::print("{}", stream.dataAsString());
        fmt::print(std::cerr, "{} bits extracted\n", stream.tell());
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
