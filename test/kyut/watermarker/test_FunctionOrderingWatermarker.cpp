#include <kyut/watermarker/FunctionOrderingWatermarker.hpp>

#include <boost/test/unit_test.hpp>

#include <wasm-io.h>

#include <kyut/BitStreamWriter.hpp>
#include <kyut/CircularBitStreamReader.hpp>

#ifndef KYUT_TEST_SOURCE_DIR
#define KYUT_TEST_SOURCE_DIR "."
#endif

BOOST_AUTO_TEST_SUITE(kyut)
BOOST_AUTO_TEST_SUITE(watermarker)

BOOST_AUTO_TEST_CASE(embed_function_ordering) {
    wasm::Module module;
    wasm::ModuleReader{}.read(KYUT_TEST_SOURCE_DIR "/example/test1.wast", module);

    BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{6});
    BOOST_REQUIRE_EQUAL(module.functions[0]->name, "g1");
    BOOST_REQUIRE_EQUAL(module.functions[1]->name, "g2");
    BOOST_REQUIRE_EQUAL(module.functions[2]->name, "g3");
    BOOST_REQUIRE_EQUAL(module.functions[3]->name, "f1");
    BOOST_REQUIRE_EQUAL(module.functions[4]->name, "f2");
    BOOST_REQUIRE_EQUAL(module.functions[5]->name, "f3");

    // Embed 0b00
    {
        CircularBitStreamReader s{{0b0000'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});
        BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{6});
        BOOST_REQUIRE_EQUAL(module.functions[0]->name, "g1");
        BOOST_REQUIRE_EQUAL(module.functions[1]->name, "g2");
        BOOST_REQUIRE_EQUAL(module.functions[2]->name, "g3");
        BOOST_REQUIRE_EQUAL(module.functions[3]->name, "f1");
        BOOST_REQUIRE_EQUAL(module.functions[4]->name, "f2");
        BOOST_REQUIRE_EQUAL(module.functions[5]->name, "f3");
    }

    // Embed 0b01
    {
        CircularBitStreamReader s{{0b0100'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});
        BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{6});
        BOOST_REQUIRE_EQUAL(module.functions[0]->name, "g1");
        BOOST_REQUIRE_EQUAL(module.functions[1]->name, "g2");
        BOOST_REQUIRE_EQUAL(module.functions[2]->name, "g3");
        BOOST_REQUIRE_EQUAL(module.functions[3]->name, "f2");
        BOOST_REQUIRE_EQUAL(module.functions[4]->name, "f1");
        BOOST_REQUIRE_EQUAL(module.functions[5]->name, "f3");
    }

    // Embed 0b10
    {
        CircularBitStreamReader s{{0b1000'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});
        BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{6});
        BOOST_REQUIRE_EQUAL(module.functions[0]->name, "g1");
        BOOST_REQUIRE_EQUAL(module.functions[1]->name, "g2");
        BOOST_REQUIRE_EQUAL(module.functions[2]->name, "g3");
        BOOST_REQUIRE_EQUAL(module.functions[3]->name, "f3");
        BOOST_REQUIRE_EQUAL(module.functions[4]->name, "f1");
        BOOST_REQUIRE_EQUAL(module.functions[5]->name, "f2");
    }

    // Embed 0b11
    {
        CircularBitStreamReader s{{0b1100'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});
        BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{6});
        BOOST_REQUIRE_EQUAL(module.functions[0]->name, "g1");
        BOOST_REQUIRE_EQUAL(module.functions[1]->name, "g2");
        BOOST_REQUIRE_EQUAL(module.functions[2]->name, "g3");
        BOOST_REQUIRE_EQUAL(module.functions[3]->name, "f1");
        BOOST_REQUIRE_EQUAL(module.functions[4]->name, "f3");
        BOOST_REQUIRE_EQUAL(module.functions[5]->name, "f2");
    }
}

BOOST_AUTO_TEST_CASE(extract_function_ordering) {
    wasm::Module module;
    wasm::ModuleReader{}.read(KYUT_TEST_SOURCE_DIR "/example/test1.wast", module);

    // Embed 0b00
    {
        CircularBitStreamReader s{{0b0000'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});

        BitStreamWriter w;
        const auto numBitsExtracted = extractFunctionOrdering(module, w, 10);

        BOOST_REQUIRE_EQUAL(numBitsExtracted, std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.tell(), std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.data().size(), std::size_t{1});
        BOOST_REQUIRE_EQUAL(w.data()[0], std::uint8_t{0b0000'0000});
    }

    // Embed 0b01
    {
        CircularBitStreamReader s{{0b0100'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});

        BitStreamWriter w;
        const auto numBitsExtracted = extractFunctionOrdering(module, w, 10);

        BOOST_REQUIRE_EQUAL(numBitsExtracted, std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.tell(), std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.data().size(), std::size_t{1});
        BOOST_REQUIRE_EQUAL(w.data()[0], std::uint8_t{0b0100'0000});
    }

    // Embed 0b10
    {
        CircularBitStreamReader s{{0b1000'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});

        BitStreamWriter w;
        const auto numBitsExtracted = extractFunctionOrdering(module, w, 10);

        BOOST_REQUIRE_EQUAL(numBitsExtracted, std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.tell(), std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.data().size(), std::size_t{1});
        BOOST_REQUIRE_EQUAL(w.data()[0], std::uint8_t{0b1000'0000});
    }

    // Embed 0b11
    {
        CircularBitStreamReader s{{0b1100'0000}};
        const auto numBitsEmbedded = embedFunctionOrdering(module, s, 10);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{2});

        BitStreamWriter w;
        const auto numBitsExtracted = extractFunctionOrdering(module, w, 10);

        BOOST_REQUIRE_EQUAL(numBitsExtracted, std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.tell(), std::size_t{2});
        BOOST_REQUIRE_EQUAL(w.data().size(), std::size_t{1});
        BOOST_REQUIRE_EQUAL(w.data()[0], std::uint8_t{0b1100'0000});
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
