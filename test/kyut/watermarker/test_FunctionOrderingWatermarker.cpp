#include <kyut/watermarker/FunctionOrderingWatermarker.hpp>

#include <boost/test/unit_test.hpp>

#include <wasm-io.h>

#include <kyut/CircularBitStreamReader.hpp>

BOOST_AUTO_TEST_SUITE(kyut)

BOOST_AUTO_TEST_SUITE(watermarker)

BOOST_AUTO_TEST_CASE(function_ordering_watermarker) {
    wasm::Module module;
    wasm::ModuleReader{}.read("../example/test1.wast", module);

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

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
