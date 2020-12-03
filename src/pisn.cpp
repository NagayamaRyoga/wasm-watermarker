#include <fmt/printf.h>
#include "cmdline.h"
#include "kyut/methods/ExportReordering.hpp"
#include "kyut/methods/FunctionReordering.hpp"
#include "wasm-io.h"

namespace {
    const std::string program_name = "pisn";
    const std::string version = "0.1.0";
} // namespace

int main(int argc, char* argv[]) {
    cmdline::parser options{};

    options.add("help", 'h', "Print help message");
    options.add("version", 'v', "Print version");

    options.add<std::string>("method", 'm', "Embedding method (function-reordering, export-reordering)", true, "", cmdline::oneof<std::string>("function-reordering", "export-reordering"));
    options.add<std::size_t>("chunk-size", 'c', "Chunk size [2~20]", false, 20, cmdline::range<std::size_t>(2, 20));
    options.add<std::string>("dump", 0, "Output format (ascii, hex)", false, "ascii", cmdline::oneof<std::string>("ascii", "hex"));

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
    const auto method = options.get<std::string>("method");
    const auto chunk_size = options.get<std::size_t>("chunk-size");
    const auto dump_format = options.get<std::string>("dump");

    try {
        wasm::Module module{};
        wasm::ModuleReader{}.read(input, module);

        kyut::BitStreamWriter w{};

        std::size_t size_bits;
        if (method == "function-reordering") {
            size_bits = kyut::methods::function_reordering::extract(w, module, chunk_size);
        } else if (method == "export-reordering") {
            size_bits = kyut::methods::export_reordering::extract(w, module, chunk_size);
        } else {
            WASM_UNREACHABLE(("unknown method: " + method).c_str());
        }

        fmt::print("{} bits\n", size_bits);

        if (dump_format == "ascii") {
            fmt::print("{}", w.data_as_str());
        } else if (dump_format == "hex") {
            for (const auto& byte : w.data()) {
                fmt::print("{:02X}", byte);
            }
        } else {
            WASM_UNREACHABLE(("unknown dump format: " + dump_format).c_str());
        }
    } catch (const std::exception& e) {
        fmt::print(std::cerr, "error: {}\n", e.what());
        std::exit(EXIT_FAILURE);
    } catch (const wasm::ParseException& e) {
        e.dump(std::cerr);
        std::exit(EXIT_FAILURE);
    }
}
