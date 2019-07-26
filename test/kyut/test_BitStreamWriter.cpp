#include <kyut/BitStreamWriter.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(kyut)
BOOST_AUTO_TEST_SUITE(bit_stream_writer)

BOOST_AUTO_TEST_CASE(write_bit) {
    BitStreamWriter s;

    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{0});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{0});

    s.writeBit(true);
    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{1});
    BOOST_REQUIRE_EQUAL(s.data()[0], std::uint8_t{0b1000'0000});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{1});

    s.writeBit(true);
    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{1});
    BOOST_REQUIRE_EQUAL(s.data()[0], std::uint8_t{0b1100'0000});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{2});

    s.writeBit(false);
    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{1});
    BOOST_REQUIRE_EQUAL(s.data()[0], std::uint8_t{0b1100'0000});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{3});

    s.writeBit(false);
    s.writeBit(true);
    s.writeBit(false);
    s.writeBit(true);
    s.writeBit(true);
    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{1});
    BOOST_REQUIRE_EQUAL(s.data()[0], std::uint8_t{0b1100'1011});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{8});
}

BOOST_AUTO_TEST_CASE(write) {
    BitStreamWriter s;

    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{0});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{0});

    s.write(std::uint16_t{0xabcd}, 16);
    BOOST_REQUIRE_EQUAL(s.data().size(), std::size_t{2});
    BOOST_REQUIRE_EQUAL(s.data()[0], std::uint8_t{0xab});
    BOOST_REQUIRE_EQUAL(s.data()[1], std::uint8_t{0xcd});
    BOOST_REQUIRE_EQUAL(s.tell(), std::size_t{16});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
