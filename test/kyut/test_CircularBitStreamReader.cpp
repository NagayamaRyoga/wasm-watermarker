#include <kyut/CircularBitStreamReader.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(kyut)
BOOST_AUTO_TEST_SUITE(circular_bit_stream_reader)

BOOST_AUTO_TEST_CASE(read_bit) {
    constexpr std::uint8_t data[1] = {0x89};
    CircularBitStreamReader reader{std::begin(data), std::end(data)};

    // 0x89 == 0b1000'1001
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{0});
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{1});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{2});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{3});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{4});
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{5});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{6});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{7});
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);

    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{8});
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{9});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{10});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{11});
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{12});
}

BOOST_AUTO_TEST_CASE(read) {
    constexpr std::uint8_t data[4] = {0x89, 0xab, 0xcd, 0xef};
    CircularBitStreamReader reader{std::begin(data), std::end(data)};

    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{0});
    BOOST_REQUIRE_EQUAL(reader.read<std::uint32_t>(4), std::uint32_t{0x8});
    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{4});
    BOOST_REQUIRE_EQUAL(reader.read<std::uint32_t>(4), std::uint32_t{0x9});

    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{8});
    BOOST_REQUIRE_EQUAL(reader.read<std::uint32_t>(8), std::uint32_t{0xab});

    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{16});
    BOOST_REQUIRE_EQUAL(reader.read<std::uint32_t>(16), std::uint32_t{0xcdef});

    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{32});
    BOOST_REQUIRE_EQUAL(reader.read<std::uint32_t>(24), std::uint32_t{0x89abcd});

    BOOST_REQUIRE_EQUAL(reader.tell(), std::size_t{56});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
