#include <fmt/printf.h>
#include "cmdline.h"
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
    options.add<std::string>("method", 'm', "Embedding method (function-ordering)", true, "", cmdline::oneof<std::string>("function-ordering"));
    options.add<std::size_t>("chunk-size", 'c', "Chunk size [2~20]", false, 20, cmdline::range<std::size_t>(2, 20));

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

    if (options.rest().empty()) {
        // No input file specified.
        fmt::print(std::cerr, "no input file\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    const auto input = options.rest()[0];
    const auto output = options.get<std::string>("output");
    const auto method = options.get<std::string>("method");
    [[maybe_unused]] const auto chunk_size = options.get<std::size_t>("chunk-size");

    try {
        wasm::Module module{};
        wasm::ModuleReader{}.read(input, module);
    } catch (const std::exception& e) {
        fmt::print(std::cerr, "error: {}\n", e.what());
        std::exit(EXIT_FAILURE);
    } catch (const wasm::ParseException& e) {
        e.dump(std::cerr);
        std::exit(EXIT_FAILURE);
    }
}
