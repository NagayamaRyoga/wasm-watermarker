#include "kyut/BitStreamWriter.hpp"

#include <gtest/gtest.h>

TEST(kyut, BitStreamWriter) {
    kyut::BitStreamWriter w{};

    EXPECT_EQ(w.position_bits(), 0);
    EXPECT_EQ(w.data_as_str(), "");

    w.write(0x0A, 4);

    EXPECT_EQ(w.position_bits(), 4);
    EXPECT_EQ(w.data_as_str(), "\xA0");

    w.write(0xBC, 8);

    EXPECT_EQ(w.position_bits(), 12);
    EXPECT_EQ(w.data_as_str(), "\xAB\xC0");

    w.write(0xDEF, 12);

    EXPECT_EQ(w.position_bits(), 24);
    EXPECT_EQ(w.data_as_str(), "\xAB\xCD\xEF");
}
