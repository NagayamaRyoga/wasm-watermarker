#include "kyut/CircularBitStreamReader.hpp"

#include <gtest/gtest.h>

TEST(kyut, CircularBitStreamReader) {
    kyut::CircularBitStreamReader r{"\x89\xAB\xCD\xEF"};
    std::uint64_t a;

    EXPECT_EQ(r.size_bytes(), 4);
    EXPECT_EQ(r.size_bits(), 32);

    EXPECT_EQ(r.position_bits(), 0);

    a = r.read(4);
    EXPECT_EQ(a, 0x8);
    EXPECT_EQ(r.position_bits(), 4);

    a = r.read(12);
    EXPECT_EQ(a, 0x9AB);
    EXPECT_EQ(r.position_bits(), 16);

    a = r.read(20);
    EXPECT_EQ(a, 0xCDEF8);
    EXPECT_EQ(r.position_bits(), 4);
}
