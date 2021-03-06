#include <fmt/printf.h>
#include "cmdline.h"
#include "kyut/methods/ExportReordering.hpp"
#include "kyut/methods/FunctionReordering.hpp"
#include "kyut/methods/OperandSwapping.hpp"
#include "wasm-io.h"

namespace {
    const std::string program_name = "snpi";
    const std::string version = "0.1.0";
} // namespace

int main(int argc, char* argv[]) {
    cmdline::parser options{};

    options.add("help", 'h', "Print help message");
    options.add("version", 'v', "Print version");

    options.add<std::string>("output", 'o', "Output filename", true);
    options.add<std::string>("method", 'm', "Embedding method (function-reorder, export-reorder, operand-swap, null)", true, "", cmdline::oneof<std::string>("function-reorder", "export-reorder", "operand-swap", "null"));
    options.add<std::string>("watermark", 'w', "Watermark to embed", true);
    options.add<std::size_t>("chunk-size", 'c', "Chunk size [2~20]", false, 20, cmdline::range<std::size_t>(2, 20));
    options.add<std::size_t>("limit", 'l', "Embedding limit", false, std::size_t(-1));
    options.add("debug", 'd', "Preserve debug info");

    options.set_program_name(program_name);
    options.footer("filename");

    // Parse command line arguments.
    // Exit the program if help flag is specified or arguments are invalid.
    options.parse_check(argc, argv);

    if (options.exist("version")) {
        // Show the program version.
        fmt::print("{} v{}\n", program_name, version);
        std::exit(EXIT_SUCCESS);
    }

    if (options.get<std::string>("watermark").empty()) {
        // Zero-length watermark.
        fmt::print(std::cerr, "no watermark\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    if (options.rest().size() == 0) {
        // No input file specified.
        fmt::print(std::cerr, "no input file\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    if (options.rest().size() > 1) {
        // Too many input files.
        fmt::print(std::cerr, "too many input files\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    const auto input = options.rest()[0];
    const auto output = options.get<std::string>("output");
    const auto method = options.get<std::string>("method");
    const auto watermark = options.get<std::string>("watermark");
    const auto chunk_size = options.get<std::size_t>("chunk-size");
    const auto limit = options.get<std::size_t>("limit");
    const auto preserve_debug = options.exist("debug");

    try {
        wasm::Module module{};
        wasm::ModuleReader{}.read(input, module);

        kyut::CircularBitStreamReader r{watermark};

        std::size_t size_bits;
        if (method == "function-reorder") {
            size_bits = kyut::methods::function_reordering::embed(r, module, limit, chunk_size);
        } else if (method == "export-reorder") {
            size_bits = kyut::methods::export_reordering::embed(r, module, limit, chunk_size);
        } else if (method == "operand-swap") {
            size_bits = kyut::methods::operand_swapping::embed(r, module, limit);
        } else if (method == "null") {
            size_bits = 0; /* Don't do anything */
        } else {
            WASM_UNREACHABLE(("unknown method: " + method).c_str());
        }

        wasm::ModuleWriter w{};
        w.setDebugInfo(preserve_debug);
        w.writeBinary(module, output);

        fmt::print("{} bits\n", size_bits);
    } catch (const std::exception& e) {
        fmt::print(std::cerr, "error: {}\n", e.what());
        std::exit(EXIT_FAILURE);
    } catch (const wasm::ParseException& e) {
        e.dump(std::cerr);
        std::exit(EXIT_FAILURE);
    }
}
