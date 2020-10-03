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

namespace {
    void check_extract(
        std::string data,
        std::size_t chunk_size,
        std::size_t expected_size_bits_extracted,
        std::string_view expected_watermark_extracted) {
        kyut::BitStreamWriter w{};

        const auto size_bits = kyut::extract_by_ordering(
            w,
            chunk_size,
            std::begin(data),
            std::end(data),
            std::less<>{});

        EXPECT_EQ(size_bits, expected_size_bits_extracted);
        EXPECT_EQ(w.position_bits(), size_bits);
        EXPECT_EQ(w.data_as_str(), expected_watermark_extracted);
    }
} // namespace

TEST(kyut_Ordering, extract_by_ordering) {
    using namespace std::string_view_literals;

    check_extract("1234", 20, 4, "\x00"sv);
    check_extract("2134", 20, 4, "\x10"sv);
    check_extract("3214", 20, 4, "\x20"sv);
    check_extract("4231", 20, 4, "\x30"sv);
    check_extract("1324", 20, 4, "\x40"sv);
    check_extract("2314", 20, 4, "\x50"sv);
}