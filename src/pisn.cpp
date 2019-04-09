#include <iostream>

#include <fmt/ostream.h>
#include <wasm-io.h>

#include "kyut/pass/operand_swapping_extractor.hpp"

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            fmt::print(std::cerr,
                       "WebAssembly digital watermark extractor.\n"
                       "usage: pisn <input file>\n");

            return 1;
        }

        wasm::Module module;
        wasm::ModuleReader{}.read(argv[1], module);

        auto writer = kyut::BitWriter{};

        kyut::pass::extractWatermarkOperandSwapping(module, writer);

        for (const auto byte : writer.bytes()) {
            fmt::print("{:02X} ", byte);
        }
    } catch (wasm::ParseException &e) {
        fmt::print(std::cerr, "parse error\n");
        e.dump(std::cerr);

        return 1;
    } catch (const std::exception &e) {
        fmt::print(std::cerr, "error: {}\n", e.what());

        return 1;
    }
}
