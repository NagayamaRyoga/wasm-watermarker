#ifndef INCLUDE_kyut_bit_writer_hpp
#define INCLUDE_kyut_bit_writer_hpp

#include <cassert>
#include <cstdint>

#include <vector>

namespace kyut {
    class BitWriter {
    public:
        explicit BitWriter(std::size_t reserved = 256)
            : bytes_()
            , pos_bits_(0) {
            bytes_.reserve(reserved);
        }

        BitWriter(const BitWriter &) = delete;
        BitWriter(BitWriter &&) = delete;

        BitWriter &operator=(const BitWriter &) = delete;
        BitWriter &operator=(BitWriter &&) = delete;

        ~BitWriter() noexcept = default;

        [[nodiscard]] const std::vector<std::uint8_t> &bytes() const noexcept {
            return bytes_;
        }

        [[nodiscard]] std::size_t size_bytes() const noexcept {
            return bytes_.size();
        }

        [[nodiscard]] std::size_t size_bits() const noexcept {
            return size_bytes() * 8 + pos_bits_ % 8;
        }

        void write_bit(bool bit) {
            if (pos_bits_ % 8 == 0) {
                bytes_.emplace_back(std::uint8_t{0x00});
            }

            if (bit) {
                bytes_.back() |= 1 << (pos_bits_ % 8);
            }

            pos_bits_++;
        }

        void write(std::uint64_t bits, std::size_t size_bits) {
            assert(size_bits < 64);

            for (std::size_t i = 0; i < size_bits; i++) {
                write_bit((bits & (1 << i)) != 0);
            }
        }

    private:
        std::vector<std::uint8_t> bytes_;
        std::size_t pos_bits_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_bit_writer_hpp
