#include <kyut/watermarker/OperandSwappingWatermarker.hpp>

#include <boost/test/unit_test.hpp>

#include <wasm-io.h>

#include <kyut/BitStreamWriter.hpp>
#include <kyut/CircularBitStreamReader.hpp>

#ifndef KYUT_TEST_SOURCE_DIR
#define KYUT_TEST_SOURCE_DIR "."
#endif

BOOST_AUTO_TEST_SUITE(kyut)
BOOST_AUTO_TEST_SUITE(watermarker)

BOOST_AUTO_TEST_CASE(embed_operand_swapping) {
    wasm::Module module;
    wasm::ModuleReader{}.read(KYUT_TEST_SOURCE_DIR "/example/test2.wast", module);

    BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{3});
    BOOST_REQUIRE_EQUAL(module.functions[0]->name, "f1");
    BOOST_REQUIRE_EQUAL(module.functions[1]->name, "f2");
    BOOST_REQUIRE_EQUAL(module.functions[2]->name, "f3");

    // Embed 0b0000'00
    {
        CircularBitStreamReader s{{0b0000'0000}};
        const auto numBitsEmbedded = embedOperandSwapping(module, s);

        BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{6});
        BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{3});
        BOOST_REQUIRE_EQUAL(module.functions[0]->name, "f1");
        BOOST_REQUIRE_EQUAL(module.functions[1]->name, "f2");
        BOOST_REQUIRE_EQUAL(module.functions[2]->name, "f3");

        // f1
        {
            const auto bin = module.functions[0]->body->cast<wasm::Binary>();

            BOOST_REQUIRE_EQUAL(bin->left->cast<wasm::Const>()->value.geti32(), 1);
            BOOST_REQUIRE_EQUAL(bin->right->cast<wasm::Const>()->value.geti32(), 2);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END() BOOST_AUTO_TEST_SUITE_END()
