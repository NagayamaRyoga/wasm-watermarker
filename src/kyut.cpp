#include <cstdio>

#include <fmt/format.h>
#include <wasm-io.h>

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            fmt::print(stderr,
                       "WebAssembly digital watermarker.\n"
                       "usage: kyut <input file> <watermark>\n");

            return 1;
        }

        wasm::Module module;
        wasm::ModuleReader{}.readBinary(argv[1], module);
    } catch (const std::exception &e) {
        fmt::print(stderr, "error: {}\n", e.what());

        return 1;
    }
}
