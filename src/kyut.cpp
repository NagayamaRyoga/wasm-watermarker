#include <iostream>

#include <fmt/ostream.h>
#include <wasm-io.h>

#include "kyut/pass/operand_swapping_watermarker.hpp"

int main(int argc, char *argv[]) {
    try {
        if (argc != 4) {
            fmt::print(std::cerr,
                       "WebAssembly digital watermarker.\n"
                       "usage: kyut <input file> <watermark> <output file>\n");

            return 1;
        }

        wasm::Module module;
        wasm::ModuleReader{}.read(argv[1], module);

        const auto stream = kyut::CircularBitStream::from_string(argv[2]);

        kyut::pass::embedWatermarkOperandSwapping(module, *stream);

        wasm::ModuleWriter{}.write(module, argv[3]);
    } catch (wasm::ParseException &e) {
        fmt::print(std::cerr, "parse error\n");
        e.dump(std::cerr);

        return 1;
    } catch (const std::exception &e) {
        fmt::print(std::cerr, "error: {}\n", e.what());

        return 1;
    }
}
