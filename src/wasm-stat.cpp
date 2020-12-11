#include <fmt/printf.h>
#include "cmdline.h"
#include "wasm-io.h"

namespace {
    const std::string program_name = "wasm-stat";
    const std::string version = "0.1.0";
} // namespace

int main(int argc, char* argv[]) {
    cmdline::parser options{};

    options.add("help", 'h', "Print help message");
    options.add("version", 'v', "Print version");
    options.add("quiet", 'q', "Only display numbers");

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
    const bool quiet = options.exist("quiet");

    try {
        wasm::Module module{};
        wasm::ModuleReader{}.read(input, module);

        if (!quiet) {
            fmt::print(
                "functions: {}\n"
                "exports: {}\n",
                module.functions.size(),
                module.exports.size());
        } else {
            fmt::print(
                "{}\t{}\n",
                module.functions.size(),
                module.exports.size());
        }
    } catch (const std::exception& e) {
        fmt::print(std::cerr, "error: {}\n", e.what());
        std::exit(EXIT_FAILURE);
    } catch (const wasm::ParseException& e) {
        e.dump(std::cerr);
        std::exit(EXIT_FAILURE);
    }
}
