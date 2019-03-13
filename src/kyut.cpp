#include <iostream>

#include <wasm-io.h>

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "WebAssembly digital watermarker." << std::endl;
            std::cerr << "usage: kyut <input file> <watermark>" << std::endl;

            return 1;
        }

        wasm::Module module;
        wasm::ModuleReader{}.readBinary(argv[1], module);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';

        return 1;
    }
}
