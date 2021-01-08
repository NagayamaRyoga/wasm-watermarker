#include <fmt/printf.h>
#include "cmdline.h"
#include "wasm-io.h"

namespace {
    const std::string program_name = "kyuk";
    const std::string version = "0.1.0";
} // namespace

int main(int argc, char* argv[]) {
    cmdline::parser options{};

    options.add("help", 'h', "Print help message");
    options.add("version", 'v', "Print version");

    options.add<std::string>("output", 'o', "Output filename", true);
    options.add<std::string>("watermark", 'w', "Watermark to embed", true);
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
    const auto watermark = options.get<std::string>("watermark");
    const auto preserve_debug = options.exist("debug");

    try {
        wasm::Module module{};
        wasm::ModuleReader{}.read(input, module);

        // TODO: Embedding

        wasm::ModuleWriter w{};
        w.setDebugInfo(preserve_debug);
        w.writeBinary(module, output);
    } catch (const std::exception& e) {
        fmt::print(std::cerr, "error: {}\n", e.what());
        std::exit(EXIT_FAILURE);
    } catch (const wasm::ParseException& e) {
        e.dump(std::cerr);
        std::exit(EXIT_FAILURE);
    }
}
