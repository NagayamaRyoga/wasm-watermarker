#include <kyut/BitStreamReader.hpp>

#include <boost/test/unit_test.hpp>

namespace {
    constexpr std::size_t operator""_zu(unsigned long long x) noexcept {
        return static_cast<std::size_t>(x);
    }
    constexpr std::uint64_t operator""_u64(unsigned long long x) noexcept {
        return static_cast<std::uint64_t>(x);
    }
} // namespace

BOOST_AUTO_TEST_SUITE(kyut)

BOOST_AUTO_TEST_SUITE(bit_stream_reader)

BOOST_AUTO_TEST_CASE(read_bit) {
    constexpr std::uint8_t data[1] = {0x89};
    BitStreamReader reader{std::begin(data), std::end(data)};

    // 0x89 == 0b1000'1001
    BOOST_REQUIRE_EQUAL(reader.tell(), 0_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);
    BOOST_REQUIRE_EQUAL(reader.tell(), 1_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 2_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 3_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 4_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);
    BOOST_REQUIRE_EQUAL(reader.tell(), 5_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 6_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 7_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);

    BOOST_REQUIRE_EQUAL(reader.tell(), 0_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), true);
    BOOST_REQUIRE_EQUAL(reader.tell(), 1_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 2_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 3_zu);
    BOOST_REQUIRE_EQUAL(reader.readBit(), false);
    BOOST_REQUIRE_EQUAL(reader.tell(), 4_zu);
}

BOOST_AUTO_TEST_CASE(read) {
    constexpr std::uint8_t data[4] = {0x89, 0xab, 0xcd, 0xef};
    BitStreamReader reader{std::begin(data), std::end(data)};

    BOOST_REQUIRE_EQUAL(reader.tell(), 0_zu);
    BOOST_REQUIRE_EQUAL(reader.read(4), 0x8_u64);
    BOOST_REQUIRE_EQUAL(reader.tell(), 4_zu);
    BOOST_REQUIRE_EQUAL(reader.read(4), 0x9_u64);

    BOOST_REQUIRE_EQUAL(reader.tell(), 8_zu);
    BOOST_REQUIRE_EQUAL(reader.read(8), 0xab_u64);

    BOOST_REQUIRE_EQUAL(reader.tell(), 16_zu);
    BOOST_REQUIRE_EQUAL(reader.read(16), 0xcdef_u64);

    BOOST_REQUIRE_EQUAL(reader.tell(), 0_zu);
    BOOST_REQUIRE_EQUAL(reader.read(24), 0x89abcd_u64);

    BOOST_REQUIRE_EQUAL(reader.tell(), 24_zu);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
