#include "kyut/Ordering.hpp"

#include <gtest/gtest.h>

namespace {
    void check_embed(
        std::string data,
        std::string_view watermark,
        std::size_t chunk_size,
        std::size_t expected_size_bits_embedded,
        std::string_view expected_reordered_data) {
        kyut::CircularBitStreamReader r{watermark};

        const auto size_bits = kyut::embed_by_ordering(
            r,
            chunk_size,
            std::begin(data),
            std::end(data),
            std::less<>{});

        EXPECT_EQ(size_bits, expected_size_bits_embedded);
        EXPECT_EQ(data, expected_reordered_data);
    }
} // namespace

TEST(kyut_Ordering, embed_by_ordering) {
    using namespace std::string_view_literals;

    check_embed("1234", "\x00"sv, 20, 4, "1234");
    check_embed("4321", "\x00"sv, 20, 4, "1234");

    check_embed("1234", "\x10"sv, 20, 4, "2134");
    check_embed("1234", "\x20"sv, 20, 4, "3214");
    check_embed("1234", "\x30"sv, 20, 4, "4231");
    check_embed("1234", "\x40"sv, 20, 4, "1324");
    check_embed("1234", "\x50"sv, 20, 4, "2314");

    check_embed("2314", "\x50"sv, 20, 4, "2314");
    check_embed("2314", "\x00"sv, 20, 4, "1234");
}
