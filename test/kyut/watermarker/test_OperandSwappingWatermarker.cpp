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

BOOST_AUTO_TEST_CASE(embed_operand_swapping_000000) {
    wasm::Module module;
    wasm::ModuleReader{}.read(KYUT_TEST_SOURCE_DIR "/example/test2.wast", module);

    BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{3});
    BOOST_REQUIRE_EQUAL(module.functions[0]->name, "f1");
    BOOST_REQUIRE_EQUAL(module.functions[1]->name, "f2");
    BOOST_REQUIRE_EQUAL(module.functions[2]->name, "f3");

    // Embed 0b0000'00
    CircularBitStreamReader s{{0b0000'0000}};
    const auto numBitsEmbedded = embedOperandSwapping(module, s);

    BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{6});
    BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{3});
    BOOST_REQUIRE_EQUAL(module.functions[0]->name, "f1");
    BOOST_REQUIRE_EQUAL(module.functions[1]->name, "f2");
    BOOST_REQUIRE_EQUAL(module.functions[2]->name, "f3");

    // f1
    {
        const auto body = module.functions[0]->body->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(body->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(body->left->cast<wasm::Const>()->value.geti32(), 1);
        BOOST_REQUIRE_EQUAL(body->right->cast<wasm::Const>()->value.geti32(), 2);
    }
    // f2
    {
        const auto body = module.functions[1]->body->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(body->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(body->left->cast<wasm::Const>()->value.geti32(), 3);

        const auto r = body->right->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(r->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(r->left->cast<wasm::Const>()->value.geti32(), 1);
        BOOST_REQUIRE_EQUAL(r->right->cast<wasm::Const>()->value.geti32(), 2);
    }
    // f3
    {
        const auto body = module.functions[2]->body->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(body->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(body->left->is<wasm::Binary>(), true);
        BOOST_REQUIRE_EQUAL(body->right->is<wasm::Binary>(), true);

        const auto l = body->left->cast<wasm::Binary>();
        const auto r = body->right->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(l->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(l->left->cast<wasm::Const>()->value.geti32(), 1);
        BOOST_REQUIRE_EQUAL(l->right->cast<wasm::Const>()->value.geti32(), 2);

        BOOST_REQUIRE_EQUAL(r->op, wasm::GtSInt32);
        BOOST_REQUIRE_EQUAL(r->left->cast<wasm::Const>()->value.geti32(), 3);
        BOOST_REQUIRE_EQUAL(r->right->cast<wasm::Const>()->value.geti32(), 4);
    }
}

BOOST_AUTO_TEST_CASE(embed_operand_swapping_110111) {
    wasm::Module module;
    wasm::ModuleReader{}.read(KYUT_TEST_SOURCE_DIR "/example/test2.wast", module);

    BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{3});
    BOOST_REQUIRE_EQUAL(module.functions[0]->name, "f1");
    BOOST_REQUIRE_EQUAL(module.functions[1]->name, "f2");
    BOOST_REQUIRE_EQUAL(module.functions[2]->name, "f3");

    // Embed 0b1101'11
    CircularBitStreamReader s{{0b1101'1100}};
    const auto numBitsEmbedded = embedOperandSwapping(module, s);

    BOOST_REQUIRE_EQUAL(numBitsEmbedded, std::size_t{6});
    BOOST_REQUIRE_EQUAL(module.functions.size(), std::size_t{3});
    BOOST_REQUIRE_EQUAL(module.functions[0]->name, "f1");
    BOOST_REQUIRE_EQUAL(module.functions[1]->name, "f2");
    BOOST_REQUIRE_EQUAL(module.functions[2]->name, "f3");

    // f1
    {
        const auto body = module.functions[0]->body->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(body->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(body->left->cast<wasm::Const>()->value.geti32(), 2);
        BOOST_REQUIRE_EQUAL(body->right->cast<wasm::Const>()->value.geti32(), 1);
    }
    // f2
    {
        const auto body = module.functions[1]->body->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(body->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(body->left->cast<wasm::Const>()->value.geti32(), 3);

        const auto r = body->right->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(r->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(r->left->cast<wasm::Const>()->value.geti32(), 2);
        BOOST_REQUIRE_EQUAL(r->right->cast<wasm::Const>()->value.geti32(), 1);
    }
    // f3
    {
        const auto body = module.functions[2]->body->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(body->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(body->left->is<wasm::Binary>(), true);
        BOOST_REQUIRE_EQUAL(body->right->is<wasm::Binary>(), true);

        const auto l = body->left->cast<wasm::Binary>();
        const auto r = body->right->cast<wasm::Binary>();

        BOOST_REQUIRE_EQUAL(l->op, wasm::LtSInt32);
        BOOST_REQUIRE_EQUAL(l->left->cast<wasm::Const>()->value.geti32(), 4);
        BOOST_REQUIRE_EQUAL(l->right->cast<wasm::Const>()->value.geti32(), 3);

        BOOST_REQUIRE_EQUAL(r->op, wasm::AddInt32);
        BOOST_REQUIRE_EQUAL(r->left->cast<wasm::Const>()->value.geti32(), 2);
        BOOST_REQUIRE_EQUAL(r->right->cast<wasm::Const>()->value.geti32(), 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
