#include <fmt/printf.h>

#include <wasm-io.h>

#include "kyut/BitStreamWriter.hpp"
#include "kyut/watermarker/FunctionOrderingWatermarker.hpp"

int main(int argc, char *argv[]) {
    // Parse command line options
    if (argc != 2) {
        fmt::print(std::cerr, "{} <input file>\n", argv[0]);

        return 1;
    }

    const std::string inputFile = argv[1];

    try {
        // Read the input module
        wasm::Module module;
        wasm::ModuleReader{}.read(inputFile, module);

        // Embed watermarks
        kyut::BitStreamWriter stream;
        kyut::watermarker::extractFunctionOrdering(module, stream, 10);

        // Output watermarks extracted
        std::cout << stream.dataAsString();
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
